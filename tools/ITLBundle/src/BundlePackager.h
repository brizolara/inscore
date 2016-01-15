#ifndef BUNDLEGENERATOR_H
#define BUNDLEGENERATOR_H

#include <string>

#include "ParsedData.h"
#include "QArchive.h"

namespace itlbundle{

class BundlePackager
{

	std::map<std::string, std::string> fNamesMap;
	ParsedData& fInputData;

public:
	static bool bundle(ParsedData& scripts, const std::string& outputPath, bool overwrite=false);
protected:

	BundlePackager(ParsedData& inputData):fInputData(inputData){}

	void mapNames();
	void setupArchive(qarchive::SQArchive& archive);
	std::string generateScript(std::string scriptName);

	bool writeArchive(qarchive::SQArchive& archive, const std::string& outputPath, bool overwrite = false);

private:
	static inline bool ignoreCmd(std::string itlCmd);

};

} // End namespace

#endif // BUNDLEGENERATOR_H