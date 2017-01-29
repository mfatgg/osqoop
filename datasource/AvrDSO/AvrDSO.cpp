/*

Osqoop, an open source software oscilloscope.
Copyright (C) 2006 Stephane Magnenat <stephane at magnenat dot net>
Laboratory of Digital Systems http://www.eig.ch/labsynum.htm
Engineering School of Geneva http://www.eig.ch

See AUTHORS for more details about other contributors.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

#include <QtCore>
#include <QDialog>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include "AvrDSO.h"
#include "com_unix.h"
#include <AvrDSO.moc>

//! Dialog box for choosing sound input
class AvrDSOInputsDialog : public QDialog
{
public:
	QListWidget *AvrDSOInputsList; //!< the list holding available AvrDSO inputs

	//! Creates the widgets, especially the list of available data sources, which is filled from the dataSources parameter
	AvrDSOInputsDialog(const QStringList &items)
	{
		QVBoxLayout *layout = new QVBoxLayout(this);

		layout->addWidget(new QLabel(tr("Please choose a AvrDSO input")));

		AvrDSOInputsList = new QListWidget();
		AvrDSOInputsList->addItems(items);
		layout->addWidget(AvrDSOInputsList);

		connect(AvrDSOInputsList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), SLOT(accept()));
	}
};

QString AvrDSODataSourceDescription::name() const
{
	return "Avr DSO";
}

QString AvrDSODataSourceDescription::description() const
{
	return "DSO connected through serial port";
}

DataSource *AvrDSODataSourceDescription::create() const
{
	return new AvrDSODataSource(this);
}


AvrDSODataSource::AvrDSODataSource(const DataSourceDescription *description) :
	DataSource(description)
{
	// Get the number of Digital Audio In devices in this computer
    QStringList inputList;
	DIR *dp;
    struct dirent *ep;
    dp = opendir ("/dev/");
    if (dp != NULL)
    {
        while ((ep = readdir (dp)))
        {
            if ((strncmp (ep->d_name, "ttyS", 4) == 0) || (strncmp (ep->d_name, "ttyUSB", 6) == 0) || (strncmp (ep->d_name, "serial", 6) == 0) || (strncmp (ep->d_name, "cuad", 4) == 0))
                inputList << (QString ("/dev/") + QString (ep->d_name));
        }
        (void) closedir (dp);
        inputList.sort();
    }
	/*if (inputList.empty())
		return false;*/

    // If required, display a dialog with inputs names
    if (inputList.size() > 1)
	{
	    AvrDSOInputsDialog avrdsoInputsDialog(inputList);
        int ret = avrdsoInputsDialog.exec();
        if (ret != QDialog::Rejected)
            sourceDevice = avrdsoInputsDialog.AvrDSOInputsList->currentItem()->text();
        else
            sourceDevice = "";
    }
	else
		sourceDevice = "";

    //printf("%s\n", sourceDevice.toAscii().data());
    com_open (sourceDevice.toAscii().data(), 115200);
    usleep (1000);
    com_puts ("RESET\r");
    usleep (5000);
    com_puts ("AC\r");
    usleep (1000);
    com_puts ("OFFSET 128\r");
    usleep (1000);
}

AvrDSODataSource::~AvrDSODataSource()
{
	com_close ();
}

unsigned AvrDSODataSource::getRawData(std::valarray<std::valarray<signed short> > *data)
{
    //unsigned long mysleep;
    //char divstr[20];

	Q_ASSERT(data->size() >= 1);

    //com_puts ("RESET\r");
    //usleep (25000);
    /*com_puts ("AC\r");
    com_puts ("TRIGFH\r");
    com_puts ("TRIGOFF 128\r");
    com_puts ("OFFSET 128\r");*/
    //printf ("%u %u\n", timescale, data->size());
    /*switch (avrdiv)
    {
        case 0:
            mysleep = 1000;
            break;
        case 1:
            mysleep = 32*1000;
            break;
        case 2:
            mysleep = 256*1000;
            break;
        default:
            mysleep = 10000;
            break;
    }
    snprintf(divstr, sizeof(divstr), "DIV %u,%u\r", avrdiv, avrsteps-1);
    printf ("%s\n", divstr);
    com_puts (divstr);
    usleep (mysleep);*/
    com_puts ("DIV2 0\r");
    usleep (2000);
    com_puts ("GET2 512\r");
    //usleep (mysleep);
    usleep (1000);

	for (size_t sample = 0; sample < 512; sample++)
	{
		for (size_t channel = 0; channel < 1; channel++)
		{
            (*data)[channel][sample] = (signed short)(com_getc(0)) - 128;
		}
	}
	// Time to wait in microseconds:
	return 1000000 / (5000 / 512);
}

unsigned AvrDSODataSource::getSamplesAmount(unsigned timescale)
{
    /*unsigned div,steps,samples;

    div = 0;
    steps = ((timescale * 100 * 10 / 20) / 400) + 1;
    samples = (timescale * 100 * 10 / 20) / steps;
    if (steps > (32768 / 400))
    {
        div = 1;
        steps = (((timescale * 100 * 10 / 20) / 400) / 32) + 1;
        samples = (timescale * 100 * 10 / 20) / (steps * 32);
    }
    if (steps > (32768 / 400))
    {
        div = 2;
        steps = (((timescale * 100 * 10 / 20) / 400) / 256) + 1;
        samples = (timescale * 100 * 10 / 20) / (steps * 256);
    }

    //printf ("div:%u steps:%u samples:%u\n", div,steps,samples);

    avrdiv = div;
    avrsteps = steps;
    avrsamples = samples;

    return avrsamples-1;*/
    if ((timescale*5*10) < 500)
        return timescale*5*10;
    else
        return 500;
}

unsigned AvrDSODataSource::inputCount() const
{
	return 1;
}

unsigned AvrDSODataSource::samplingRate() const
{
	return 500000;
}

bool AvrDSODataSource::isNonContinuous()
{
    return true;
}

unsigned AvrDSODataSource::unitPerVoltCount() const
{
    return 30;
}

Q_EXPORT_PLUGIN(AvrDSODataSourceDescription)
