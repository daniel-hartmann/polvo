#pragma once

#include "MainComponent.h"

class MainWindow : public juce::DocumentWindow
{
public:
    MainWindow(juce::String name, std::unique_ptr<MainComponent> mainComponent);
    ~MainWindow() override;

    void closeButtonPressed() override;

    std::unique_ptr<MainComponent> mainComponent;
};
