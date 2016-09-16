////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (apply)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "Files.h"
#include "OutputStreamImpostor.h"

// dataset
#include "Example.h"
#include "RowDataset.h"

// common
#include "DataLoadArguments.h"
#include "DataSaveArguments.h"
#include "DataLoaders.h"
#include "LoadModel.h"
#include "MapLoadArguments.h"
#include "MapSaveArguments.h"

// model
#include "InputNode.h"
#include "Model.h"
#include "DynamicMap.h"

// stl
#include <iostream>
#include <memory>
#include <stdexcept>
#include <tuple>

using namespace emll;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedDataSaveArguments dataSaveArguments;
        common::ParsedMapLoadArguments mapLoadArguments;
        common::ParsedMapSaveArguments mapSaveArguments;

        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(dataSaveArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(mapSaveArguments);

        bool verbose = false;
        commandLineParser.AddOption(verbose, "verbose", "v", "Verbose mode", false);

        // TODO:
        //
        // apply with model + input/output PortElements-type representations
        // apply with map + optional input name & output names (if not specified, use first input / output found)

        // Also need a way to write out a map, given a model + inputs/outputs
        // Maybe have outputMap and outputData options... if you are only outputting the map, you
        // don't need to specify an input dataset

        // parse command line
        commandLineParser.Parse();

        if (verbose)
        {
            std::cout << "Apply" << std::endl;
            std::cout << commandLineParser.GetCurrentValuesString() << std::endl;
        }

        // load dataset
        if (verbose) std::cout << "Loading data ..." << std::endl;
        auto dataset = common::GetRowDataset<dataset::DenseRowDataset>(dataLoadArguments);
        size_t numColumns = dataLoadArguments.parsedDataDimension;

        // load map
        if (verbose) std::cout << "Loading map ..." << std::endl;
        auto mapFilename = "";
        auto map = common::LoadMap(mapFilename);

        // Get dataset iterator
        auto datasetIterator = dataset.GetIterator();
        // get output stream
        auto outputStream = dataSaveArguments.outputDataStream;
        while (datasetIterator.IsValid())
        {
            auto row = datasetIterator.Get();
            map.SetInput<double>("input", row.GetDataVector());
            auto output = map.ComputeOutput<dataset::DoubleDataVector>("output");
            auto mappedRow = dataset::DenseSupervisedExample{ output, row.GetMetadata() };
            mappedRow.Print(outputStream);
            datasetIterator.Next();
        }
    }
    catch (const utilities::CommandLineParserPrintHelpException& exception)
    {
        std::cout << exception.GetHelpText() << std::endl;
        return 0;
    }
    catch (const utilities::CommandLineParserErrorException& exception)
    {
        std::cerr << "Command line parse error:" << std::endl;
        for (const auto& error : exception.GetParseErrors())
        {
            std::cerr << error.GetMessage() << std::endl;
        }
        return 1;
    }
    catch (const utilities::Exception& exception)
    {
        std::cerr << "exception: " << exception.GetMessage() << std::endl;
        return 1;
    }

    return 0;
}
