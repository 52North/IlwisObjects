/*IlwisObjects is a framework for analysis, processing and visualization of remote sensing and gis data
Copyright (C) 2018  52n North

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include "kernel.h"
#include "raster.h"
#include "pixeliterator.h"
#include "symboltable.h"
#include "ilwisoperation.h"
#include "opencv.hpp"
#include "opencvhelper.h"
#include "opencvoperation.h"
#include "mergepictures.h"

using namespace Ilwis;
using namespace OpenCV;

REGISTER_OPERATION(MergePictures)

MergePictures::MergePictures()
{
}

MergePictures::MergePictures(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

Ilwis::OperationImplementation *MergePictures::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
	return new MergePictures(metaid, expr);
}

bool MergePictures::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
	if (_prepState == sNOTPREPARED)
		if ((_prepState = prepare(ctx, symTable)) != sPREPARED)
			return false;

	cv::Mat3b imgInit = cv::imread(_inputPaths[0].toStdString().c_str());
	cv::Mat3b outputImage(imgInit.rows * _rowsPerImage, imgInit.cols * _imagesPerRow, cv::Vec3b(0, 0, 0));

	int idx = 0;
	for (int y = 0; y < _rowsPerImage; ++y) {
		for (int x = 0; x < _imagesPerRow; ++x) {
			cv::Mat3b img = cv::imread(_inputPaths[idx].toStdString().c_str());
			img.copyTo(outputImage(cv::Rect(x*256, y*256, img.cols, img.rows)));
			++idx;
		}
	}
	cv::imwrite(_outputPath.toStdString().c_str(), outputImage);
	try {
		return true;

	}
	catch (cv::Exception& ex) {
		ERROR0(QString::fromStdString(ex.msg));
	}
	return false;
}

Ilwis::OperationImplementation::State MergePictures::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
	try {
		OperationImplementation::prepare(ctx, st);
		_outputPath = _expression.input<QString>(0);
		_inputPaths = _expression.input<QString>(1).split("|");
		if (_inputPaths.size() == 0) {
			kernel()->issues()->log("Number of input images is zero");
			return sPREPAREFAILED;
		}

		_imagesPerRow = _expression.input<int>(2);
		_rowsPerImage = _expression.input<int>(3);
		if (_imagesPerRow * _rowsPerImage != _inputPaths.size()) {
			kernel()->issues()->log("Number of input images doesn't match the sizes given");
			return sPREPAREFAILED;
		}
	
		return sPREPARED;

	}
	catch (const CheckExpressionError& err) {
		ERROR0(err.message());
	}
	return sPREPAREFAILED;
}

quint64 MergePictures::createMetadata()
{
	OperationResource operation({ "ilwis://operations/mergepictures" }, "opencv");
	operation.setSyntax("mergepictures(outputpath,inputfiles, xsize, ysize)");
	operation.setDescription(TR("Merges a set of pictures (not maps) into one unified picture. The pictures must be of equal size"));
	operation.setInParameterCount({ 4 });
	operation.addInParameter(0, itSTRING, TR("Output path"), TR("The local file path for the output"));
	operation.addInParameter(1, itSTRING, TR("Picture List"), TR("A list of file paths to local pictures seperated by '|' characters. The pictures must be organized per row"));
	operation.addInParameter(2, itINTEGER, TR("Number of pictures per row"), TR("How many pictures are used to fill the row of the final picture"));
	operation.addInParameter(3, itINTEGER, TR("Number of rows"), TR("How rows of pictures are used to fill the the final picture"));
	operation.setOutParameterCount({ 0 });
	operation.setKeywords("image processing");
	operation.parameterNeedsQuotes(1);

	mastercatalog()->addItems({ operation });
	return operation.id();
}
