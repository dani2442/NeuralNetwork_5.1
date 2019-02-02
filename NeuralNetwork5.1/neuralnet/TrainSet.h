#pragma once
#include <vector>


class TrainSet
{
public:
	TrainSet(){}
	TrainSet(size_t INPUT_SIZE, size_t OUTPUT_SIZE);
	~TrainSet(){}

	size_t TRAINING_SIZE;
	size_t INPUT_SIZE;
	size_t OUTPUT_SIZE;

	void MNIST();

	void addData(const std::vector<double>& in, const std::vector<double>& target);

	TrainSet extractBatch(size_t size,size_t pos);

	const size_t size()const { return trainingSet.size(); }
	std::vector<double>& getInput(size_t index) { return trainingSet[index]; }
	std::vector<double>& getOutput(size_t index) { return labelSet[index]; }

	void setTrainingSetSize() { TRAINING_SIZE = trainingSet.size(); }

private:
	std::vector<std::vector<double>> trainingSet;
	std::vector<std::vector<double>> labelSet;
};



inline TrainSet::TrainSet(size_t INPUT_SIZE, size_t OUTPUT_SIZE):
	INPUT_SIZE(INPUT_SIZE),
	OUTPUT_SIZE(OUTPUT_SIZE),
	TRAINING_SIZE(0)
{
}

inline void TrainSet::MNIST()
{
	TRAINING_SIZE = File::read_mnist("images/test-images.d24",trainingSet);

	std::vector<__int8> temp(TRAINING_SIZE);
	File::read_mnist_label("images/test-labels.d24", temp);

	labelSet.resize(TRAINING_SIZE, std::vector<double>(10));
	for (size_t i = 0; i < TRAINING_SIZE; i++) {
		labelSet[i][temp[i]] = 1.0;
	}

	INPUT_SIZE = trainingSet[0].size();
	OUTPUT_SIZE = labelSet[0].size();
}

inline void TrainSet::addData(const std::vector<double>& in, const std::vector<double>& target)
{
	trainingSet.push_back(in);
	labelSet.push_back(target);
}

inline TrainSet TrainSet::extractBatch(size_t size,size_t pos)
{
	if (size < this->size()) {
		TrainSet set(INPUT_SIZE, OUTPUT_SIZE);

		for (size_t i = 0; i < size;i++) {
			if (i + pos >= TRAINING_SIZE) break;
			set.addData(getInput(i + pos), getOutput(i + pos));
		}
		set.setTrainingSetSize();
		return set;
	}
	else return *this;
	
}