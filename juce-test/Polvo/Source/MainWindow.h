#pragma once

#include "MainComponent.h"
#include "PolvoTheme.h"

class MainWindow : public juce::DocumentWindow
{
public:
    MainWindow(juce::String name, std::unique_ptr<MainComponent> mainComponent);
    ~MainWindow() override;

    PolvoLookAndFeel lookAndFeel;

    void closeButtonPressed() override;

    std::unique_ptr<MainComponent> mainComponent;
};
