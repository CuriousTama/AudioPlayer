#include "FileProcessingFactory.h"
#include "FileProcessing.h"

// ----------------------------------------------
bool FileProcessingFactory::contains(const FileExtension& _fileExtension) const
{
    FileExtension fileExtension = getUnifiedString(_fileExtension);
    return m_registery.contains(fileExtension);
}

// ----------------------------------------------
std::unique_ptr<FileProcessing> FileProcessingFactory::create(const FileExtension& _fileExtension) const
{
    FileExtension fileExtension = getUnifiedString(_fileExtension);
    if (!m_registery.contains(fileExtension))
        return nullptr;

    const CreateObjectFunc& creationFunc = m_registery.at(fileExtension);
    return creationFunc();
}

// ----------------------------------------------
std::string FileProcessingFactory::getUnifiedString(std::string _str) const
{
    transform(_str.begin(), _str.end(), _str.begin(), ::tolower);
    return _str;
}
