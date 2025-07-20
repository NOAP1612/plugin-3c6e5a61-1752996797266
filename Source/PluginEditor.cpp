#include "PluginProcessor.h"
#include "PluginEditor.h"

VintageCompressorAudioProcessorEditor::VintageCompressorAudioProcessorEditor (VintageCompressorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      thresholdSliderAttachment(p.apvts, "threshold", thresholdSlider),
      ratioSliderAttachment(p.apvts, "ratio", ratioSlider),
      attackSliderAttachment(p.apvts, "attack", attackSlider),
      releaseSliderAttachment(p.apvts, "release", releaseSlider),
      kneeSliderAttachment(p.apvts, "knee", kneeSlider),
      tubeDriveSliderAttachment(p.apvts, "tubeDrive", tubeDriveSlider),
      makeupSliderAttachment(p.apvts, "makeup", makeupSlider)
{
    setSize (500, 340);

    // Threshold
    thresholdSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    thresholdSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
    thresholdSlider.setSkewFactorFromMidPoint(-24.0f);
    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.attachToComponent(&thresholdSlider, false);
    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(thresholdLabel);

    // Ratio
    ratioSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    ratioSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    ratioSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::yellow);
    ratioLabel.setText("Ratio", juce::dontSendNotification);
    ratioLabel.attachToComponent(&ratioSlider, false);
    addAndMakeVisible(ratioSlider);
    addAndMakeVisible(ratioLabel);

    // Attack
    attackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    attackSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightblue);
    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.attachToComponent(&attackSlider, false);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(attackLabel);

    // Release
    releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    releaseSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightgreen);
    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.attachToComponent(&releaseSlider, false);
    addAndMakeVisible(releaseSlider);
    addAndMakeVisible(releaseLabel);

    // Knee
    kneeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    kneeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    kneeSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::pink);
    kneeLabel.setText("Knee", juce::dontSendNotification);
    kneeLabel.attachToComponent(&kneeSlider, false);
    addAndMakeVisible(kneeSlider);
    addAndMakeVisible(kneeLabel);

    // Tube Drive
    tubeDriveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    tubeDriveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    tubeDriveSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::red);
    tubeDriveLabel.setText("Tube Drive", juce::dontSendNotification);
    tubeDriveLabel.attachToComponent(&tubeDriveSlider, false);
    addAndMakeVisible(tubeDriveSlider);
    addAndMakeVisible(tubeDriveLabel);

    // Makeup Gain
    makeupSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    makeupSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    makeupSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::white);
    makeupLabel.setText("Makeup", juce::dontSendNotification);
    makeupLabel.attachToComponent(&makeupSlider, false);
    addAndMakeVisible(makeupSlider);
    addAndMakeVisible(makeupLabel);

    // Title
    titleLabel.setText("Vintage Compressor", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(26.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::goldenrod);
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Tube Warmth subtitle
    tubeWarmthLabel.setText("Tube Warmth", juce::dontSendNotification);
    tubeWarmthLabel.setFont(juce::Font(16.0f, juce::Font::italic));
    tubeWarmthLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    tubeWarmthLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(tubeWarmthLabel);
}

VintageCompressorAudioProcessorEditor::~VintageCompressorAudioProcessorEditor()
{
}

void VintageCompressorAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour(30, 20, 10));
    g.setColour (juce::Colours::goldenrod);
    g.drawRoundedRectangle(10, 10, getWidth()-20, getHeight()-20, 20.0f, 2.0f);
}

void VintageCompressorAudioProcessorEditor::resized()
{
    const int sliderW = 80;
    const int sliderH = 100;
    const int margin = 20;
    const int spacing = 10;

    titleLabel.setBounds(0, 10, getWidth(), 40);
    tubeWarmthLabel.setBounds(0, 50, getWidth(), 20);

    int y = 80;
    int x = margin;

    thresholdSlider.setBounds(x, y, sliderW, sliderH);
    x += sliderW + spacing;
    ratioSlider.setBounds(x, y, sliderW, sliderH);
    x += sliderW + spacing;
    attackSlider.setBounds(x, y, sliderW, sliderH);
    x += sliderW + spacing;
    releaseSlider.setBounds(x, y, sliderW, sliderH);

    x = margin;
    y += sliderH + 30;
    kneeSlider.setBounds(x, y, sliderW, sliderH);
    x += sliderW + spacing;
    tubeDriveSlider.setBounds(x, y, sliderW, sliderH);
    x += sliderW + spacing;
    makeupSlider.setBounds(x, y, sliderW, sliderH);
}

```

```cmake
#