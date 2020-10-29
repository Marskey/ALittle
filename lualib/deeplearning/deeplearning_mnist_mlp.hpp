#ifndef DEEPLEARNING_MNIST_MLP_INCLUDED
#define DEEPLEARNING_MNIST_MLP_INCLUDED

#include "dynet/training.h"
#include "dynet/expr.h"
#include "dynet/io.h"
#include "dynet/model.h"
#include "other/mlp.h"

#include "deeplearning_model.hpp"
#include "carp_surface.hpp"

class DeeplearningMnistMLPModel : public DeeplearningModel
{
public:
	DeeplearningMnistMLPModel() : m_trainer(m_collection)
		, m_mlp(m_collection, std::vector<dynet::Layer>({
		        dynet::Layer(/* input_dim */ IMAGE_SIZE* IMAGE_SIZE, /* output_dim */ 512, /* activation */ dynet::RELU, /* dropout_rate */ 0.2f),
		        dynet::Layer(/* input_dim */ 512, /* output_dim */ 512, /* activation */ dynet::RELU, /* dropout_rate */ 0.2f),
		        dynet::Layer(/* input_dim */ 512, /* output_dim */ 10, /* activation */ dynet::LINEAR, /* dropout_rate */ 0.0f)
			}))
	{
		m_trainer.clip_threshold *= BATCH_SIZE;
	}

public:
	void Init(const char* model_path, const char* train_image, const char* train_labels)
	{
		if (model_path != nullptr)
		{
			dynet::TextFileLoader loader(model_path);
			loader.populate(m_collection);
		}

		m_train_image_path.clear();
		m_train_labels_path.clear();
		if (train_image) m_train_image_path = train_image;
		if (train_labels) m_train_labels_path = train_labels;

		m_mnist_train.clear();
		m_mnist_train_labels.clear();
		m_train_order.clear();
		m_train_index = 0;

		m_total_train_count = 0;
		m_cur_train_count = 0;
		m_train_round = 0;
		m_cur_right_count = 0;

		// ����
		m_trainer.restart();
	}

	void TrainInit() override
	{
		if (m_train_image_path.size() && m_train_labels_path.size())
		{
			ReadMnist(m_train_image_path, m_mnist_train);
			for (auto& trains : m_mnist_train)
			{
				for (auto& value : trains)
					value /= 255.0f;
			}
			ReadMnistLabels(m_train_labels_path, m_mnist_train_labels);

			// ������������
			auto num_batches = m_mnist_train.size() / BATCH_SIZE - 1;
			m_train_order.resize(num_batches);
			for (int i = 0; i < (int)num_batches; ++i) m_train_order[i] = i;
			std::random_shuffle(m_train_order.begin(), m_train_order.end());

			m_total_train_count = (int)m_mnist_train.size();
		}
	}

	void TrainRelease() override
	{
		m_mnist_train.clear();
		m_mnist_train_labels.clear();
	}

	double Training() override
	{
		if (m_train_order.empty()) return 0;
		if (dynet::get_number_of_active_graphs() > 0) return 0;
		
		// ����dropout
		m_mlp.enable_dropout();
		
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
			cur_batch[idx] = input(cg, { IMAGE_SIZE * IMAGE_SIZE }, m_mnist_train[id + idx]);
			cur_labels[idx] = m_mnist_train_labels[id + idx];
		}
		// ����������ϲ�Ϊһ������
		dynet::Expression x_batch = reshape(concatenate_cols(cur_batch), dynet::Dim({ IMAGE_SIZE * IMAGE_SIZE }, bsize));

		// �������
		dynet::Expression y = m_mlp.run(x_batch, cg);

		// Ԥ������ͳ����ȷ����
		std::vector<float> probs = as_vector(cg.forward(y));
		for (unsigned index = 0; index < bsize; ++index)
		{
			unsigned count = (unsigned)probs.size() / bsize;
			unsigned offset = index * count;
			// Get argmax
			unsigned argmax = 0;
			for (unsigned i = 1; i < count; ++i) {
				if (probs[offset + i] > probs[offset + argmax])
					argmax = i;
			}

			if (argmax == cur_labels[index])
				++m_cur_right_count;
		}
		
		// ���� ��������Ȼ ģ��
		dynet::Expression losses = pickneglogsoftmax(y, cur_labels);
		// ������ʧ
		dynet::Expression loss_expr = sum_batches(losses);
		
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
			m_cur_right_count = 0;
			m_train_index = 0;
		}
		return loss / bsize;
	}

	int Output(size_t address)
	{
		if (dynet::get_number_of_active_graphs() > 0) return 0;
		
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
				data[i] = (float)surface->GetGray(i);
		}
		else
		{
			CarpSurface new_surface(IMAGE_SIZE, IMAGE_SIZE);
			new_surface.ScaleFrom(surface, nullptr, nullptr);

			data.resize(IMAGE_SIZE * IMAGE_SIZE, 0.0f);
			for (unsigned i = 0; i < IMAGE_SIZE * IMAGE_SIZE; ++i)
				data[i] = (float)new_surface.GetGray(i);
		}

		m_mlp.disable_dropout();

		// ������̬ͼ
		dynet::ComputationGraph cg;
		// ���õ�ǰ����
		dynet::Expression x = input(cg, { IMAGE_SIZE* IMAGE_SIZE }, data);
		// ���� ��������Ȼ ģ�ͣ�������Ԥ��
		return (int)m_mlp.predict(x, cg);
	}

private:
	dynet::AdamTrainer m_trainer;
	
private:
	dynet::MLP m_mlp;

	std::vector<std::vector<float>> m_mnist_train;
	std::vector<unsigned> m_mnist_train_labels;
	std::vector<int> m_train_order;
	unsigned int m_train_index = 0;

	std::string m_train_image_path;
	std::string m_train_labels_path;

private:
	static const unsigned BATCH_SIZE = 128;
	static const unsigned IMAGE_SIZE = 28;
};

#endif