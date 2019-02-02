#include "neuralnet/NeuralNetwork5_0.h"


#include <cmath>

int main() {

	NeuralNetwork net(std::vector<size_t>{ 784,70,35,10 });
	//NeuralNetwork net2("dani.dp");

	TrainSet set;
	set.MNIST();

	net.train(set, 10, 1);

	net.Write_NN("output.dp"); // outputs the neural network structure and data
}
