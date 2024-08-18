#ifndef EDITPROPERTY_H
#define EDITPROPERTY_H
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class EditProperty : public QDialog
{
    Q_OBJECT
public:
    EditProperty(const QString& title, const QString& property, QWidget *parent = nullptr);
    ~EditProperty();
    QString getEditedProperty();
    void setToEditProperty(const QString& property);
    bool isOk();
private slots:
    void okClicked();
    void cancelClicked();
private:
    bool isCancelOrOk;
    QLabel *labelProperty;
    QLineEdit *editProperty;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QHBoxLayout *topLayout;
    QHBoxLayout *bottomLayout;
    QVBoxLayout *mainLayout;
};

#endif // EDITPROPERTY_H
