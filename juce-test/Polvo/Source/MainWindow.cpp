#include "MainWindow.h"

MainWindow::MainWindow(juce::String name, std::unique_ptr<MainComponent> mainComponent)
    : DocumentWindow(name,
                     juce::Desktop::getInstance().getDefaultLookAndFeel()
                     .findColour(juce::ResizableWindow::backgroundColourId),
                     DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(true);
    setContentOwned(mainComponent.release(), true);

#if JUCE_IOS || JUCE_ANDROID
    setFullScreen(true);
#else
    setResizable(true, true);
    centreWithSize(getWidth(), getHeight());
#endif

    juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName("Arimo");

    setVisible(true);
    setBackgroundColour(juce::Colour(0xffc0c0c0));
}

MainWindow::~MainWindow() {}

void MainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
