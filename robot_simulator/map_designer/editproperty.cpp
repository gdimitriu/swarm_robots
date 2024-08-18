#include "editproperty.h"
#include <QtGui>

EditProperty::EditProperty(const QString& title, const QString& property, QWidget *parent) : QDialog(parent)
{
    labelProperty = new QLabel(property);
    labelProperty->setAlignment(Qt::AlignCenter);
    editProperty = new QLineEdit;
    labelProperty->setBuddy(editProperty);
    okButton = new QPushButton(tr("Ok"));
    cancelButton = new QPushButton(tr("Cancel"));
    connect(okButton, SIGNAL(clicked(bool)), this, SLOT(okClicked()));
    connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(cancelClicked()));
    topLayout = new QHBoxLayout;
    topLayout->addWidget(labelProperty);
    topLayout->addWidget(editProperty);
    bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(okButton);
    bottomLayout->addWidget(cancelButton);
    mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);
    setLayout(mainLayout);
    setWindowTitle(title);
    setFixedHeight(sizeHint().height());
    isCancelOrOk = true;
}

EditProperty::~EditProperty() {
    delete labelProperty;
    delete editProperty;
    delete okButton;
    delete cancelButton;
    delete topLayout;
    delete bottomLayout;
    delete mainLayout;
}
void EditProperty::okClicked()
{
    isCancelOrOk = false;
    close();
}
void EditProperty::cancelClicked()
{
    isCancelOrOk = true;
    close();
}

bool EditProperty::isOk()
{
    return !isCancelOrOk;
}

QString EditProperty::getEditedProperty()
{
    return editProperty->text();
}

void EditProperty::setToEditProperty(const QString& property)
{
    editProperty->setText(property);
}
