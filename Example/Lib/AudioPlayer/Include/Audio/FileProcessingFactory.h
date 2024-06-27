#pragma once
#include "AudioHelper.h"
#include <memory>
#include <string>
#include <unordered_map>

class FileProcessing;

class FileProcessingFactory final
{
public:
    using FileExtension = std::string;
    using CreateObjectFunc = std::unique_ptr<FileProcessing> (*)();
    using Registery = std::unordered_map<FileExtension, CreateObjectFunc>;

    template<AudioHelper::DerivedOrSameType<FileProcessing> T>
    void registerExtention(const FileExtension& _fileExtension);

    bool contains(const FileExtension& _fileExtension) const;
    std::unique_ptr<FileProcessing> create(const FileExtension& _fileExtension) const;

private:
    template<AudioHelper::DerivedOrSameType<FileProcessing> T>
    static std::unique_ptr<FileProcessing> createObject();

    std::string getUnifiedString(std::string _str) const;

    Registery m_registery;
};

/////////////
template<AudioHelper::DerivedOrSameType<FileProcessing> T>
void FileProcessingFactory::registerExtention(const FileExtension& _fileExtension)
{
    FileExtension fileExtension = getUnifiedString(_fileExtension);
    m_registery[fileExtension] = FileProcessingFactory::createObject<T>;
}

/////////////
template<AudioHelper::DerivedOrSameType<FileProcessing> T>
std::unique_ptr<FileProcessing> FileProcessingFactory::createObject()
{
    return std::make_unique<T>();
}