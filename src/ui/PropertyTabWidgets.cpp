#include "ui/PropertyTabWidgets.h"

#include <QVBoxLayout>
#include <QLabel>

PropertyPage::PropertyPage(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    this->setLayout(layout);
}


CueGeneralPage::CueGeneralPage(QWidget* parent) : PropertyPage(parent) {
    auto* label = new QLabel("General stuff idk", this);
    this->layout()->addWidget(label);
}
bool CueGeneralPage::supportsCue(Cue& cue) {
    return true;
}


TextCuePage::TextCuePage(QWidget* parent) : PropertyPage(parent) {
    auto* label = new QLabel("Text cue idk", this);
    this->layout()->addWidget(label);
}
bool TextCuePage::supportsCue(Cue& cue) {
    return true;
}



IdkRandomPage::IdkRandomPage(QWidget* parent) : PropertyPage(parent) {
    auto* label = new QLabel("Random stuff\nyeey multiple lines\nidk", this);
    this->layout()->addWidget(label);
}