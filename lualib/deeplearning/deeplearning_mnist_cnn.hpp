#ifndef DEEPLEARNING_MNIST_CNN_INCLUDED
#define DEEPLEARNING_MNIST_CNN_INCLUDED

#include "dynet/training.h"
#include "dynet/expr.h"
#include "dynet/io.h"
#include "dynet/model.h"

#include "deeplearning_model.hpp"
#include "carp_surface.hpp"

class DeeplearningMnistCNNModel : public DeeplearningModel
{
public:
	DeeplearningMnistCNNModel() : m_trainer(m_collection)
	{
		m_pConv1 = m_collection.add_parameters({ 5, 5, 1, 32 });
		m_pB1 = m_collection.add_parameters({ 32, });
		m_pConv2 = m_collection.add_parameters({ 5, 5, 32, 64 });
		m_pB2 = m_collection.add_parameters({ 64, });
		m_pW1 = m_collection.add_parameters({ HIDDEN_DIM, 7 * 7 * 64 });
		m_pB3 = m_collection.add_parameters({ HIDDEN_DIM, });
		m_pW2 = m_collection.add_parameters({ 10, HIDDEN_DIM });
	}

public:
	void Init(const char* model_path, const char* train_image, const char* train_labels)
	{
		if (model_path != nullptr)
		{
			dynet::TextFileLoader loader(model_path);
			loader.populate(m_collection);
		}
		
		m_mnist_train.clear();
		m_mnist_train_labels.clear();
		m_train_order.clear();

		// ��������
		if (train_image && train_labels)
		{
			ReadMnist(train_image, m_mnist_train);
			for (auto& trains : m_mnist_train)
			{
				for (auto& value : trains)
					value /= 255.0f;
			}
			ReadMnistLabels(train_labels, m_mnist_train_labels);
			
			// ������������
			auto num_batches = m_mnist_train.size() / BATCH_SIZE - 1;
			m_train_order.resize(num_batches);
			for (int i = 0; i < (int)num_batches; ++i) m_train_order[i] = i;
			std::random_shuffle(m_train_order.begin(), m_train_order.end());
		}
		m_train_index = 0;

		m_total_train_count = (int)m_mnist_train.size();
		m_cur_train_count = 0;
		m_train_round = 0;

		// ����
		m_trainer.restart();
	}

	dynet::Expression Build(dynet::ComputationGraph& cg, dynet::Expression x, bool dropout)
	{
		// ��ȡ��������
		auto bsize = x.dim().batch_elems();
		
		// �������
		dynet::Expression conv1 = dynet::parameter(cg, m_pConv1);
		// ƫ�ò���
		dynet::Expression b1 = dynet::parameter(cg, m_pB1);
		// ���
		dynet::Expression conv1_x = dynet::conv2d(x, conv1, b1, { 1, 1 }, false);
		// ����
		dynet::Expression relu1_x = dynet::rectify(dynet::maxpooling2d(conv1_x, { 2, 2 }, { 2, 2 }));

		// �������
		dynet::Expression conv2 = dynet::parameter(cg, m_pConv2);
		// ƫ�ò���
		dynet::Expression b2 = dynet::parameter(cg, m_pB2);
		// ���
		dynet::Expression conv2_x = dynet::conv2d(relu1_x, conv2, b2, { 1, 1 }, false);
		// ����
		dynet::Expression relu2_x = dynet::rectify(dynet::maxpooling2d(conv2_x, { 2, 2 }, { 2, 2 }));

		// ����ά��
		dynet::Expression reshape_x = dynet::reshape(relu2_x, dynet::Dim({ 7 * 7 * 64, 1 }, bsize));

		// ���Ա任
		dynet::Expression w1 = dynet::parameter(cg, m_pW1);
		dynet::Expression b3 = dynet::parameter(cg, m_pB3);
		dynet::Expression h = dynet::rectify(w1 * reshape_x + b3);

		if (dropout) h = dynet::dropout(h, DROPOUT_RATE);
		dynet::Expression w2 = dynet::parameter(cg, m_pW2);

		return w2 * h;
	}

	double Training()
	{
		if (m_train_order.empty()) return 0;
		
		// ������̬ͼ
		dynet::ComputationGraph cg;
		// ���㵱ǰ���ε��±�ʹ�С
		int id = m_train_order[m_train_index] * BATCH_SIZE;
		unsigned bsize = std::min((unsigned)m_mnist_train.size() - id, BATCH_SIZE);
		// ���õ�ǰ���ε�����
		std::vector<dynet::Expression> cur_batch(bsize);
		std::vector<unsigned> cur_labels(bsize);
		for (unsigned idx = 0; idx < bsize; ++idx)
		{
			cur_batch[idx] = input(cg, { IMAGE_SIZE, IMAGE_SIZE }, m_mnist_train[id + idx]);
			cur_labels[idx] = m_mnist_train_labels[id + idx];
		}

		// ����������ϲ�Ϊһ������
		dynet::Expression x_batch = dynet::concatenate_to_batch(cur_batch);
		// ������ز�
		dynet::Expression output = Build(cg, x_batch, true);
		// ���� ��������Ȼ ģ��
		dynet::Expression loss_expr = dynet::mean_batches(dynet::pickneglogsoftmax(output, cur_labels));
		// �ۼ���ʧֵ
		double loss = as_scalar(cg.forward(loss_expr));
		// ���򴫲�
		cg.backward(loss_expr);
		// ���²���
		m_trainer.update();

		++m_train_index;
		m_cur_train_count += bsize;
		if (m_train_index >= (int)m_train_order.size())
		{
			++m_train_round;
			m_cur_train_count = 0;
			m_train_index = 0;
		}
		return loss / bsize;
	}

	int Output(size_t address)
	{
		CarpSurface* surface = nullptr;
		memcpy(&surface, &address, sizeof(size_t));
		
		if (surface == nullptr) return -1;
		if (surface->GetWidth() == 0 || surface->GetHeight() == 0) return -1;
		
		std::vector<float> data;

		// ���ͼƬ����ָ����С��ô�ͽ�������
		if (surface->GetWidth() == IMAGE_SIZE && surface->GetHeight() == IMAGE_SIZE)
		{
			data.resize(IMAGE_SIZE * IMAGE_SIZE, 0.0f);
			for (unsigned i = 0; i < IMAGE_SIZE * IMAGE_SIZE; ++i)
				data[i] = (float)surface->GetGray(i) / 255.0f;
		}
		else
		{
			CarpSurface new_surface(IMAGE_SIZE, IMAGE_SIZE);
			new_surface.ScaleFrom(surface, nullptr, nullptr);

			data.resize(IMAGE_SIZE * IMAGE_SIZE, 0.0f);
			for (unsigned i = 0; i < IMAGE_SIZE * IMAGE_SIZE; ++i)
				data[i] = (float)new_surface.GetGray(i) / 255.0f;
		}

		// ������̬ͼ
		dynet::ComputationGraph cg;
		// ���õ�ǰ����
		dynet::Expression x = input(cg, { IMAGE_SIZE, IMAGE_SIZE }, data);
		// ������ز�
		dynet::Expression output = Build(cg, x, true);
		// Ԥ��
		dynet::Expression out = dynet::softmax(output);
		// Get values
		std::vector<float> probs = dynet::as_vector(cg.forward(out));
		// Get argmax
		unsigned argmax = 0;
		for (unsigned i = 1; i < probs.size(); ++i) {
			if (probs[i] > probs[argmax])
				argmax = i;
		}

		return argmax;
	}

private:
	dynet::SimpleSGDTrainer m_trainer;

private:
	dynet::Parameter m_pConv1;
	dynet::Parameter m_pB1;
	dynet::Parameter m_pConv2;
	dynet::Parameter m_pB2;
	dynet::Parameter m_pW1;
	dynet::Parameter m_pB3;
	dynet::Parameter m_pW2;
	
private:
	std::vector<std::vector<float>> m_mnist_train;
	std::vector<unsigned> m_mnist_train_labels;
	std::vector<int> m_train_order;
	unsigned int m_train_index = 0;

private:
	static const unsigned BATCH_SIZE = 1;
	static const unsigned HIDDEN_DIM = 1024;
	static const unsigned IMAGE_SIZE = 28;
	static constexpr float DROPOUT_RATE = 0.4f;
};

#endif