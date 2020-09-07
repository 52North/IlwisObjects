/*IlwisObjects is a framework for analysis, processing and visualization of remote sensing and gis data
Copyright (C) 2018  52n North

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <QApplication>
#include <QGLFormat>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtQml>
#include <QQuickStyle>
 
#include "startilwis.h"          
   
int main(int argc, char *argv[])               
{
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv); 
	QQuickStyle::setStyle("Material");
		   
#ifdef _WIN32                
		 
        bool ver20 = false;              
        if (QGLFormat::hasOpenGL()) {  
            QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();
            if (flags >= QGLFormat::OpenGL_Version_2_0)
                ver20 = true;     
        }   
        if (ver20) {                              

#endif


            QQmlApplicationEngine engine;   
            qmlRegisterType<StartIlwis>("StartIlwis",1,0,"StartIlwis");
            StartIlwis start(0,&engine);
            QUrl url = engine.baseUrl(); 
            engine.rootContext()->setContextProperty("startilwis", &start);
			QString path = qApp->applicationDirPath();
            engine.load(QUrl("qrc:/qml/StartWindow.qml"));
            QObject *topLevel = engine.rootObjects().value(0); 
            QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel); 
            window->setDefaultAlphaBuffer(true);
            int ret =  app.exec();  
            start.stop();     
            return ret;          
			 
#ifdef _WIN32      

        } else {

            MessageBox(0, (LPCTSTR)QString("OpenGL version 2.0 or better is required.\nAre the drivers of the graphics hardware properly installed?").utf16(), (LPCTSTR)QString("ILWIS4 Desktop cannot start.").utf16(), MB_OK | MB_ICONERROR);

            return 0;
        }

#endif

}
