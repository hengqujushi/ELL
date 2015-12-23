// HorizontalLayout.cpp

#include "HorizontalLayout.h"

HorizontalLayout::HorizontalLayout(uint64 numElements, double layerHorizontalMargin, double layerMaxWidth, double elementWidth, double xElementSpacing, double elementHorizontalLeftPadding, double elementHorizontalRightPadding, double dotsWidth) : _elementXMid(numElements)
{
    // width of the layer without abbreviated format 
    double layerWidth = elementHorizontalLeftPadding + elementHorizontalRightPadding + numElements * elementWidth + (numElements-1) * xElementSpacing;
    double elementWidthPlusSpace = elementWidth + xElementSpacing;

    // non-abbreviated
    if(layerWidth <= layerMaxWidth)
    {
        _layerWidth = layerWidth;
        _numHiddenElements = 0;
        double firstElementXMiddle = layerHorizontalMargin + elementHorizontalLeftPadding + elementWidth/2.0;
        for(uint64 k = 0; k<numElements; ++k)
        {
            _elementXMid[k] = firstElementXMiddle + k * elementWidthPlusSpace;
        }
    }
    
    // abbreviated layer: shows as many visible elements as possible, followed by a gap, followed by one last element
    else
    {
        _layerWidth = layerMaxWidth;

        double elementsWidth = layerMaxWidth - elementHorizontalLeftPadding - elementHorizontalRightPadding;
        double elementsBeforeGapWidth = elementsWidth - dotsWidth - elementWidth - xElementSpacing;
        uint64 numElementsBeforeGap = uint64(elementsBeforeGapWidth / (elementWidth + xElementSpacing));
        _numHiddenElements = numElements - 1 - numElementsBeforeGap;
        
        if(elementsBeforeGapWidth <= 0 || numElementsBeforeGap == 0)
        {
            throw runtime_error("unable to visualize layer within the specified constraints (increase width, decrease dots width or element width/spacing)");
        }

        // elements before gap
        double firstElementXMiddle = layerHorizontalMargin + elementHorizontalLeftPadding + elementWidth/2.0;
        for(uint64 k = 0; k<numElementsBeforeGap; ++k)
        {
            _elementXMid[k] = firstElementXMiddle + k * elementWidthPlusSpace;
        }

        // elements represented by dots
        double dotsLeft = layerHorizontalMargin + elementHorizontalLeftPadding + numElementsBeforeGap * elementWidthPlusSpace;
        double dotsRight = layerHorizontalMargin + _layerWidth - elementHorizontalRightPadding - elementWidthPlusSpace;
        _dotsXMid = (dotsLeft + dotsRight) / 2.0;
        double hiddenElementsSpacing = (dotsRight - dotsLeft) / (_numHiddenElements -1);

        for(uint64 k = numElementsBeforeGap; k<numElements-1; ++k)
        {
            _elementXMid[k] = dotsLeft + (k- numElementsBeforeGap) * hiddenElementsSpacing;
        }

        /// element after dots
        _elementXMid[numElements-1] = dotsRight + xElementSpacing + elementWidth/2.0;
    }
}

double HorizontalLayout::GetXMid(uint64 index) const
{
    return _elementXMid[index];
}

double HorizontalLayout::GetWidth() const
{
    return _layerWidth;
}

bool HorizontalLayout::HasHidden() const
{
    return _numHiddenElements > 0;
}

bool HorizontalLayout::IsHidden(uint64 index) const
{
    uint64 numElements = _elementXMid.size();
    if (index == numElements - 1 || index < numElements - 1 - _numHiddenElements)
    {
        return false;
    }
    return true;
}

double HorizontalLayout::GetGapXMid() const
{
    return _dotsXMid;
}

