#pragma once

#include <thread>

class NeuralNetwork
{
public:
	NeuralNetwork() {}
	NeuralNetwork(std::string path);
	NeuralNetwork(std::vector<size_t> topology);
	~NeuralNetwork() {}

	std::vector<double> feedForward(std::vector<double>& input);

	void trainBatch(TrainSet& set,int i);
	void train(TrainSet& set, size_t max_epochs, size_t batch_size);

	void backropError();

	double MSE(std::vector<double> input, std::vector<double> target);
	double MSE(TrainSet& set);

	size_t epochs, MAX_EPOCHS;

	double eta = 0.1;

	std::vector<double> loss;

	size_t BATCH_SIZE;
	std::vector<size_t> TOPOLOGY;
	size_t INPUT_SIZE, OUTPUT_SIZE;
	size_t NETWORK_SIZE;

	//TrainSet batch;
	std::vector<std::thread> th;

	std::vector<Layer> layer;

	void Write_NN(std::string path);
	void Load_NN(std::string path);

private:
	size_t current = 0;
};


inline NeuralNetwork::NeuralNetwork(std::string path)
{
	Load_NN(path);
}

inline NeuralNetwork::NeuralNetwork(std::vector<size_t> topology):
	OUTPUT_SIZE(topology.back()),
	loss(topology.back()),
	INPUT_SIZE(topology[0]),
	NETWORK_SIZE(topology.size()),
	TOPOLOGY(topology)
{
	layer.push_back(Layer());
	for (size_t i = 1; i < NETWORK_SIZE-1;i++) {
		layer.push_back(Layer(topology[i - 1], topology[i],ActivationFncENUM::RELU));
	}
	layer.push_back(Layer(topology[NETWORK_SIZE-2], topology[NETWORK_SIZE-1],ActivationFncENUM::SIGLOG));
}

inline std::vector<double> NeuralNetwork::feedForward(std::vector<double>& input)
{
	if (input.size() != this->INPUT_SIZE) return {};
	layer[0].output = input;

	for (size_t i = 1; i < NETWORK_SIZE; i++) {
		layer[i].feedForward(layer[i - 1].output);
	}
	return layer[NETWORK_SIZE-1].output;
}

inline void NeuralNetwork::trainBatch(TrainSet& set,int i)
{
	std::vector<double> input = set.getInput(i); 
	std::vector<double> target = set.getOutput(i);
	if (input.size() != INPUT_SIZE || target.size() != OUTPUT_SIZE) return;

	std::vector<double> output=feedForward(input);
	for (size_t neuron = 0; neuron < output.size(); neuron++) {
		loss[neuron] += output[neuron] - target[neuron];
	}
	
}

inline void NeuralNetwork::train(TrainSet & set, size_t max_epochs, size_t batch_size)
{
	MAX_EPOCHS = max_epochs;
	BATCH_SIZE = batch_size;
	th.resize(batch_size);
	epochs = 0;

	if (set.INPUT_SIZE != INPUT_SIZE || set.OUTPUT_SIZE != OUTPUT_SIZE) return;

	while (epochs<=MAX_EPOCHS)
	{
		for (current = 0; current < set.TRAINING_SIZE; ) {

			//std::vector<std::thread> th(BATCH_SIZE);
			//batch = set.extractBatch(BATCH_SIZE,i);
			for (size_t j = 0; j < BATCH_SIZE && current<set.TRAINING_SIZE; j++,current++) {
				//th[j]=std::thread(&NeuralNetwork::trainBatch,this,j);
				this->trainBatch(set,current);
			}
			//for (size_t j = 0; j < batch.TRAINING_SIZE; j++) th[j].join();

			backropError();
			std::cout <<epochs<<"-"<<current<<"  "<< MSE(set) << std::endl;
		}
		epochs++;
	}
	
}

inline void NeuralNetwork::backropError()
{
	for (size_t neuron = 0; neuron < TOPOLOGY[NETWORK_SIZE - 1]; neuron++) {
		layer[NETWORK_SIZE - 1].error_signal[neuron] = loss[neuron] / BATCH_SIZE * layer[NETWORK_SIZE - 1].output_derivative[neuron];
	}

	for (size_t l = NETWORK_SIZE - 2; l > 0; l--) {
		for (size_t neuron = 0; neuron < TOPOLOGY[l]; neuron++) {
			double sum = 0;
			for (size_t nextNeuron = 0; nextNeuron < TOPOLOGY[l + 1]; nextNeuron++) {
				sum += layer[l+1].weight[nextNeuron][neuron] * layer[l + 1].error_signal[nextNeuron];
			}
			layer[l].error_signal[neuron] = sum * layer[l].output_derivative[neuron];
		}
	}

	for (size_t l = 1; l < NETWORK_SIZE; l++) {
		for (size_t neuron = 0; neuron < TOPOLOGY[l]; neuron++) {
			double delta = -eta * layer[l].error_signal[neuron];
			layer[l].bias[neuron] += delta;
			for (size_t prevNeuron = 0; prevNeuron < TOPOLOGY[l - 1]; prevNeuron++) {
				double deltaWeight = delta * layer[l - 1].output[prevNeuron];
				layer[l].weight[neuron][prevNeuron] += deltaWeight;
			}
		}
	}

	for (auto& a : loss) a = 0; // loss to 0
}


inline double NeuralNetwork::MSE(std::vector<double> input, std::vector<double> target)
{
	if (input.size() != INPUT_SIZE || target.size() != OUTPUT_SIZE) return 0.0;
	feedForward(input);
	double s = 0;
	for (size_t i = 0; i < target.size(); i++) {
		s += pow(target[i] - layer[NETWORK_SIZE - 1].output[i],2);
		std::cout << target[i] << "  " << layer[NETWORK_SIZE - 1].output[i]<<std::endl;
	}
	std::cout<<"/////"<<std::endl;
	return s / (2.0*target.size());
}

inline double NeuralNetwork::MSE(TrainSet & set)
{
	double s = 0;
	for (size_t i = current-BATCH_SIZE; i < current; i++) {
		s += MSE(set.getInput(i), set.getOutput(i));
	}
	return s / BATCH_SIZE;
}

inline void NeuralNetwork::Write_NN(std::string path)
{
	Document doc(kObjectType);
	Document::AllocatorType& allocator = doc.GetAllocator();
	Value Jlayers(kArrayType);
	for(size_t i=1;i<this->NETWORK_SIZE;i++){
		Value Jlayer(kObjectType);
		Jlayer.AddMember("n_neurons_pre", Value().SetInt(layer[i].weight[0].size()), allocator);
		Jlayer.AddMember("n_neurons_pos", Value().SetInt(layer[i].weight.size()), allocator);
		Value weight(kArrayType);
		for (auto& b : this->layer[i].weight) {
			for (auto&c : b) {
				weight.PushBack(Value().SetDouble(c),allocator);
			}
		}
		Jlayer.AddMember("weight", weight, allocator);
		Value bias(kArrayType);
		for (const auto& b : this->layer[i].bias) {
			bias.PushBack(Value().SetDouble(b),allocator);
		}
		Jlayer.AddMember("bias", bias, allocator);
		Jlayers.PushBack(Jlayer, allocator);
	}
	doc.AddMember("NN", Jlayers, allocator);

	char *newsource;
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	newsource=_strdup( buffer.GetString() );
	doc.Swap(Value(kObjectType).Move());
	File::write(path,newsource);
	delete[] newsource;
}

inline void NeuralNetwork::Load_NN(std::string path)
{
	char* data;
	File::read(path, data);
	Document doc;
	doc.Parse(data);
	const Value& s_weight = doc["NN"].GetArray();
	layer.resize(s_weight.Size() + 1);
	for (size_t l = 0; l < s_weight.Size();l++) {
		int neurons_pre=s_weight[l]["n_neurons_pre"].GetInt();
		int neurons_pos=s_weight[l]["n_neurons_pos"].GetInt();
		layer[l + 1].weight.resize(neurons_pos, std::vector<double>(neurons_pre));
		const auto& s_weights = s_weight[l]["weight"].GetArray();
		for (size_t i = 0; i < s_weights.Size();i++) {
			layer[l + 1].weight[i / neurons_pre][i%neurons_pre] = s_weights[i].GetDouble();
		}
		layer[l + 1].bias.resize(neurons_pos);
		const auto& s_bias = s_weight[l]["bias"].GetArray();
		for (size_t i = 0; i < s_bias.Size();i++) {
			layer[l+1].bias[i]= s_bias[i].GetDouble();
		}
	}
}


