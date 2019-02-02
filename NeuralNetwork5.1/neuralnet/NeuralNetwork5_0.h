#pragma once

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "ActivationFunctions.h"

#include <vector>
#include <stdlib.h>
#include <iostream>

class TrainSet;
class NeuralNetwork;
class File;
class Layer;


using namespace dp;
using namespace rapidjson;

#include "File.h"
#include "TrainSet.h"
#include "Layer.h"
#include "NeuralNet.h"
