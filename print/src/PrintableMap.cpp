// PrintableMap.cpp

#include "PrintableMap.h"
#include "PrintableInput.h"
#include "PrintableCoordinatewise.h"
#include "PrintableSum.h"
#include "svgHelpers.h"

using std::make_shared;

#include <string>
using std::to_string;

#include <memory>
using std::dynamic_pointer_cast;

void PrintableMap::Print(ostream & os, const CommandLineArgs& args)
{
    os << 
R"aw(
<html>
<body>

<style>
    text
    {
        font-family:    sans-serif;
    }

    rect
    {
        stroke:        #222222;
        stroke-width:    2;
    }
    
    rect.SCALE
    {
        fill:        #FF2244;
    }

    rect.SHIFT
    {
        fill:        #22AA11;
    }

    rect.SUM
    {
        fill:        #7744FF;
    }

    rect.IN
    {
        fill:        #AAAAAA;
    }

    rect.Element
    {
        fill:        white;
    }

    ellipse.Connector
    {
        fill:        #222222;
    }

    path.Edge
    {
        stroke:             #110011;
        stroke-width:       2;
        fill:               none;
        stroke-dasharray:   )aw"; 
    
    os << args.edgeStyle.dashStyle;
        
    os <<
R"aw(;
    }

    text.Layer
    {
        fill:           white;
        font:           bold 15 sans-serif;        
    }

    text.Element
    {
        fill:           black;
        font:           15 sans-serif;        
    }

    text.ElementIndex
    {
        fill:           #666666;
        font:           9 sans-serif;        
    }

</style>

<svg>

)aw";

    // print layer by layer
    double yOffset = args.layerVerticalMargin;
    for (uint64 row = 0; row < _layers.size(); ++row)
    {
        _layers[row]->ComputeLayout(args, yOffset);

        string typeName = _layers[row]->GetTypeName();
        double layerLeft = args.layerHorizontalMargin;
        double layerHeight = _layers[row]->GetHeight();
        double layerYMid = yOffset + layerHeight/2.0;

        // draw the layer rectangle
        svgRect(os, typeName, layerLeft, yOffset, args.layerCornerRadius, _layers[row]->GetWidth(), layerHeight);
        svgText(os, to_string(row), "Layer", layerLeft + 15, layerYMid);
        svgText(os, typeName, "Layer", layerLeft + 40, layerYMid, true);

        // let the layer print its contents
        _layers[row]->Print(os);

        // print edges
        if (row > 0) // skip input layer
        {
            uint64 layerSize = _layers[row]->Size();
            for (uint64 column = 0; column<layerSize; ++column)
            {
                if (!_layers[row]->IsHidden(column)) // if output is hidden, hide edge
                {
                    auto inputCoordinates = dynamic_pointer_cast<Layer>(_layers[row])->GetInputCoordinates(column);
                    while (inputCoordinates.IsValid()) // foreach incoming edge
                    {
                        auto inputCoord = inputCoordinates.Get();
                        if (!_layers[inputCoord.GetRow()]->IsHidden(inputCoord.GetColumn())) // if input is hidden, hide edge
                        {
                            svgEdge(os, _layers[inputCoord.GetRow()]->GetBeginPoint(inputCoord.GetColumn()), _layers[row]->GetEndPoint(column), args.edgeStyle.flattness);
                        }
                        inputCoordinates.Next();
                    }
                }
            }
        }

        // compute offset of next layer
        yOffset += _layers[row]->GetHeight() + args.layerVerticalSpacing;
    }

    os << 
R"aw(
</svg>
</body>
</html>
)aw";
}

void PrintableMap::Deserialize(JsonSerializer & serializer)
{
    serializer.Read("layers", _layers, PrintableMap::DeserializeLayers);
}

void PrintableMap::DeserializeLayers(JsonSerializer & serializer, shared_ptr<IPrintable>& up)
{
    auto type = serializer.Read<string>("_type");
    auto version = serializer.Read<int>("_version");

    if (type == "Input")
    {
        auto upZero = make_shared<PrintableInput>();
        upZero->Deserialize(serializer, version);
        up = upZero;
    }
    else if (type == "Scale")
    {
        auto upScale = make_shared<PrintableCoordinatewise>("SCALE");
        upScale->Deserialize(serializer, version);
        up = upScale;
    }
    else if (type == "Shift")
    {
        auto upShift = make_shared<PrintableCoordinatewise>("SHIFT");
        upShift->Deserialize(serializer, version);
        up = upShift;
    }
    else if (type == "Sum")
    {
        auto upSum = make_shared<PrintableSum>();
        upSum->Deserialize(serializer, version);
        up = upSum;
    }
    else
    {
        throw runtime_error("unidentified type in map file: " + type);
    }
}

