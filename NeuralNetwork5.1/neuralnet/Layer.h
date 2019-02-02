#pragma once

class Layer
{
public:
	Layer();
	Layer(size_t in, size_t out,ActivationFncENUM fnc);
	~Layer();

	std::vector<double> output;
	std::vector<double> output_derivative;
	std::vector<std::vector<double>> weight;
	std::vector<double> bias;
	std::vector<double> error_signal;

	ActivationFncENUM fnc;

	size_t LAYER_SIZE, PRE_LAYER_SIZE;

	void feedForward(const std::vector<double>& input);
private:

	void initValues(double constant);

	double sigmoid(const double x) { return 1.0 / (1.0 + exp(-x)); }
};

inline Layer::Layer()
{
}

inline Layer::Layer(size_t pre_layer, size_t layer,ActivationFncENUM fnc):
	output(layer),
	weight(layer,std::vector<double>(pre_layer)),
	bias(layer),
	error_signal(layer),
	output_derivative(layer),
	PRE_LAYER_SIZE(pre_layer),
	LAYER_SIZE(layer),
	fnc(fnc)
{
	double constant = sqrt(1.0 / ((double)(pre_layer + layer)));
	std::cout << constant << std::endl;
	initValues(constant);
}

inline Layer::~Layer()
{
}

inline void Layer::feedForward(const std::vector<double>& input)
{
	for (size_t neuron = 0; neuron < LAYER_SIZE; neuron++) {
		double sum = bias[neuron];
		for (size_t prevNeuron = 0; prevNeuron < PRE_LAYER_SIZE; prevNeuron++) {
			sum += input[prevNeuron] * weight[neuron][prevNeuron];
		}
		output[neuron] = (activationFnc[fnc])(sum);
		output_derivative[neuron] = (derivateActivationFnc[fnc])(output[neuron]);
	}
}

inline void Layer::initValues(double constant)
{
	// weight init
	for (auto& a : weight) {
		for (auto &w : a) {
			//w = 0.8*((double)rand() / RAND_MAX)-0.4;
			w = constant*((double)rand() / RAND_MAX)-constant/2;
			//std::cout << w << std::endl;
		}
	}

	// bias init
	for (auto &b : bias) {
		b=0.7*((double)rand() / RAND_MAX);
	}
}
