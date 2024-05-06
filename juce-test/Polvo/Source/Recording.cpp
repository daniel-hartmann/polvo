/*
  ==============================================================================

    Recording.cpp
    Created: 4 May 2024 12:24:26am
    Author:  daniel

  ==============================================================================
*/

#include "Recording.h"

Recording::Recording(const juce::File& file) : file(file),
  fileName(file.getFileNameWithoutExtension()) {}

Recording::Recording(const juce::DirectoryEntry& entry) : file(entry.getFile()) {}

const juce::String& Recording::getFileName() const { return fileName; }

void Recording::setFileName(const juce::String& newName) { fileName = newName; }
void Recording::setFile(const juce::File& newName) { file = file; }

const juce::File& Recording::getFile() const { return file; }
