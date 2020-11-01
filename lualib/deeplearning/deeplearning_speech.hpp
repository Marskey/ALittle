#ifndef DEEPLEARNING_SPEECH_INCLUDED
#define DEEPLEARNING_SPEECH_INCLUDED

#include "other/mfcc.h"
#include "deeplearning_model.hpp"
#include <fstream>

#define _MFCCData 0
#define _SpeechData 0

CARP_MESSAGE_MACRO(MFCCData, std::vector<std::vector<float>>, mfcc, std::string, sentence, std::string, phoneme);
CARP_MESSAGE_MACRO(SpeechData, std::vector<MFCCData>, data_list);

class DeeplearningSpeechModel : public DeeplearningModel
{
public:
	DeeplearningSpeechModel() : m_trainer(m_collection)
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
	bool Wav2MFCC(const char* desc_path, const char* wav_base_path, const char* out_path)
	{
		std::ifstream desc_file(desc_path);
		if (!desc_file.is_open()) return false;

		SpeechData speech_data;
		while (true)
		{
			std::string file;
			if (!std::getline(desc_file, file)) break;
			std::string sentence;
			if (!std::getline(desc_file, sentence)) break;
			std::string phoneme;
			if (!std::getline(desc_file, phoneme)) break;

			std::string wav_path = wav_base_path;
			wav_path += file;
			std::ifstream wav_file(wav_path, std::ios::binary);
			if (!wav_file.is_open()) break;

			MFCC mfcc;
			std::vector<std::vector<double>> mfcc_list;
			std::string error;
			if (!mfcc.process(wav_file, mfcc_list, error)) return false;

			speech_data.data_list.emplace_back(MFCCData());
			auto& back = speech_data.data_list.back();

			back.sentence = sentence;
			back.phoneme = phoneme;

			back.mfcc.resize(mfcc_list.size());
			for (size_t i = 0; i < mfcc_list.size(); ++i)
				for (auto& value : mfcc_list[i])
					back.mfcc[i].push_back(static_cast<float>(value));
		}

		std::vector<char> out;
		out.resize(speech_data.GetTotalSize());
		speech_data.Serialize(out.data());
		return CarpMessageWriteFactory::WriteMemoryToStdFile(out_path, out.data(), out.size());
	}

public:
	void SetTrainDataPath(const char* mfcc_path)
	{
		// ����ѵ������·��
		m_train_mfcc_path.clear();
		if (mfcc_path) m_train_mfcc_path = mfcc_path;
	}

	dynet::Expression Build(dynet::ComputationGraph& cg, dynet::Expression x, bool dropout) const
	{
		// �������
		const auto conv1 = dynet::parameter(cg, m_pConv1);
		// ƫ�ò���
		const auto b1 = dynet::parameter(cg, m_pB1);
		// ���
		const auto conv1_x = dynet::conv2d(x, conv1, b1, { 1, 1 }, false);
		// ����
		const auto relu1_x = dynet::rectify(dynet::maxpooling2d(conv1_x, { 2, 2 }, { 2, 2 }));

		// �������
		const auto conv2 = dynet::parameter(cg, m_pConv2);
		// ƫ�ò���
		const auto b2 = dynet::parameter(cg, m_pB2);
		// ���
		const auto conv2_x = dynet::conv2d(relu1_x, conv2, b2, { 1, 1 }, false);
		// ����
		const auto relu2_x = dynet::rectify(dynet::maxpooling2d(conv2_x, { 2, 2 }, { 2, 2 }));

		// ����ά��
		const auto reshape_x = dynet::reshape(relu2_x, { 7 * 7 * 64, 1 });

		// ���Ա任
		const auto w1 = dynet::parameter(cg, m_pW1);
		const auto b3 = dynet::parameter(cg, m_pB3);
		auto h = dynet::rectify(w1 * reshape_x + b3);

		if (dropout) h = dynet::dropout(h, DROPOUT_RATE);
		const auto w2 = dynet::parameter(cg, m_pW2);

		return w2 * h;
	}

	size_t TrainInit() override
	{
		m_data.data_list.clear();

		std::vector<char> out;
		if (!CarpMessageReadFactory::LoadStdFile(m_train_mfcc_path, out)) return 0;

		int result = m_data.Deserialize(out.data(), static_cast<int>(out.size()));
		if (result < CARP_MESSAGE_DR_NO_DATA) return 0;

		return m_data.data_list.size();
	}

	void TrainRelease() override
	{
		m_data.data_list.clear();
	}

	double Training(size_t index, bool& right) override
	{
		if (dynet::get_number_of_active_graphs() > 0) return 0;

		// ������̬ͼ
		dynet::ComputationGraph cg;
		// ����������ϲ�Ϊһ������
		const auto x = input(cg, { IMAGE_SIZE, IMAGE_SIZE }, m_image_list[index]);
		// ������ز�
		const auto output = Build(cg, x, true);

		// Ԥ��
		const auto label = Prediction(cg, output);
		right = label == static_cast<int>(m_label_list[index]);

		// ���� ��������Ȼ ģ��
		const auto loss_expr = dynet::pickneglogsoftmax(output, m_label_list[index]);
		// �ۼ���ʧֵ
		const double loss = as_scalar(cg.forward(loss_expr));
		// ���򴫲�
		cg.backward(loss_expr);
		// ���²���
		m_trainer.update();

		return loss;
	}

	static int Prediction(dynet::ComputationGraph& cg, dynet::Expression output)
	{
		// Ԥ��
		const auto out = dynet::softmax(output);
		// Get values
		const auto probs = dynet::as_vector(cg.forward(out));
		// Get argmax
		size_t argmax = 0;
		for (size_t i = 1; i < probs.size(); ++i) {
			if (probs[i] > probs[argmax])
				argmax = i;
		}

		return static_cast<int>(argmax);
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
				data[i] = static_cast<float>(surface->GetGray(i)) / 255.0f;
		}
		else
		{
			CarpSurface new_surface(IMAGE_SIZE, IMAGE_SIZE);
			new_surface.ScaleFrom(surface, nullptr, nullptr);

			data.resize(IMAGE_SIZE * IMAGE_SIZE, 0.0f);
			for (unsigned i = 0; i < IMAGE_SIZE * IMAGE_SIZE; ++i)
				data[i] = static_cast<float>(new_surface.GetGray(i)) / 255.0f;
		}

		// ������̬ͼ
		dynet::ComputationGraph cg;
		// ���õ�ǰ����
		const auto x = input(cg, { IMAGE_SIZE, IMAGE_SIZE }, data);
		// ������ز�
		const auto output = Build(cg, x, false);

		return Prediction(cg, output);
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
	std::string m_train_mfcc_path;

	SpeechData m_data;

private:
	static const unsigned HIDDEN_DIM = 1024;
	static const unsigned IMAGE_SIZE = 28;
	static constexpr float DROPOUT_RATE = 0.4f;
};

#endif