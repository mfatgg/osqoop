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

#ifndef __AVR_DSO_H
#define __AVR_DSO_H

#include <DataSource.h>

//! Description of AvrDSO, a simple sinus generator of various frequencies
class AvrDSODataSourceDescription : public QObject, public DataSourceDescription
{
	Q_OBJECT
	Q_INTERFACES(DataSourceDescription)

public:
	virtual QString name() const;
	virtual QString description() const;

	virtual DataSource *create() const;
};

//! AvrDSO, a digital sampling oscilloscope based on a diy homebrew project
class AvrDSODataSource : public DataSource
{
private:
	friend class AvrDSODataSourceDescription;
	AvrDSODataSource(const DataSourceDescription *description);
	QString sourceDevice;
	unsigned avrdiv, avrsteps, avrsamples;

public:
    virtual ~AvrDSODataSource();
	virtual bool init(void) { return true; }
	virtual unsigned getRawData(std::valarray<std::valarray<signed short> > *data);

    virtual unsigned getSamplesAmount(unsigned timescale);
    virtual bool isNonContinuous();

	virtual unsigned inputCount() const;
	virtual unsigned samplingRate() const;
    virtual unsigned unitPerVoltCount() const;
};


#endif
