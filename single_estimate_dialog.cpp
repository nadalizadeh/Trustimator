#include "single_estimate_dialog.h"
#include "ui_single_estimate_dialog.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include "dataset.h"
#include "estimator_base.h"

SingleEstimateDialog::SingleEstimateDialog(QWidget *parent, Dataset *ds, EstimatorBase* estm) :
        QDialog(parent), ui(new Ui::SingleEstimateDialog)
{
    ui->setupUi(this);
    this->dataset = ds;
    this->estimator = estm;
    this->createForm();
}

void SingleEstimateDialog::createForm()
{
    QVector< QMap<QString, QVariant> > schema;
    schema = dataset->getSchema();

    QFormLayout *formLayout = new QFormLayout;

    for(int i=0; i<schema.size(); i++)
    {
        QMap<QString, QVariant> & properties = schema[i];

        int col_type = properties["Type"].toInt();
        QString label = properties["Name"].toString();
        QWidget* input_field = NULL;

        switch(col_type)
        {
        case COL_TYPE_NUM:
            {
                QLineEdit * num_field = new QLineEdit(this);
                num_field->setValidator(new QDoubleValidator(-999999.0, 999999.0, 2, num_field));
                input_field = num_field;
                connect(num_field, SIGNAL(textChanged(QString)), this, SLOT(updateEstimate()));
                break;
            }
        case COL_TYPE_META:
            {
                input_field = new QLineEdit(this);
                break;
            }
        case COL_TYPE_CAT:
            {
                QComboBox* cat_field = new QComboBox(this);
                QVariantList values = properties["Values"].toList();
                for(int vi=0; vi<values.size(); vi++)
                {
                    cat_field->insertItem(0, values[vi].toString(), QVariant(vi+1));
                }
                input_field = cat_field;
                connect(cat_field, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEstimate()));
            }
        }

        input_widgets.append(input_field);
        formLayout->addRow(label, input_field);
    }

    QPushButton* closeButton = new QPushButton("&Close", this);
    closeButton->setMaximumWidth(150);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(updateEstimate()));
    formLayout->addRow(closeButton);

    setLayout(formLayout);
}

void SingleEstimateDialog::updateEstimate()
{
    QLineEdit* credit_rating = (QLineEdit*) input_widgets[input_widgets.size()-1];

    // Construct input array
    unsigned int total_num_data, num_input, num_output;
    dataset->get_training_dimensions(&total_num_data, &num_input, &num_output);

    float* input = new float[num_input];

    QVector< QMap<QString, QVariant> > schema;
    schema = dataset->getSchema();

    int input_index = 0;
    for(int i=0; i<schema.size() - 1; i++)
    {
        QMap<QString, QVariant> & properties = schema[i];
        int col_type = properties["Type"].toInt();

        switch(col_type)
        {
        case COL_TYPE_META:
            continue;

        case COL_TYPE_CAT:
            {
                QVariantList values = schema[i].value("Values").toList();
                QComboBox* cb = (QComboBox*) input_widgets[i];

                int cval = cb->currentIndex()+1;
                for(int j=0; j<values.size(); j++)
                {
                    input[input_index] = (cval == (j+1)) ? 1 : 0;
                    input_index++;
                }
                break;
            }

        case COL_TYPE_NUM:
            {
                QLineEdit* num_field = (QLineEdit*) input_widgets[i];
                input[input_index] = QVariant( num_field->text() ).toFloat();
                input_index++;
                break;
            }

        default:
            qFatal("Unhandled variable type");
        }
    }

    // Estimate using the estimator helper
    float credit = estimator->estimate(input);

    credit_rating->setText( QVariant(credit).toString() );
}

SingleEstimateDialog::~SingleEstimateDialog()
{
    delete ui;
}
