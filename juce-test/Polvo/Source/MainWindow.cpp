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
    setLookAndFeel(&lookAndFeel);

    setVisible(true);
    setBackgroundColour(PolvoColor::dark);
}

MainWindow::~MainWindow()
{
    setLookAndFeel(nullptr);
}

void MainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
