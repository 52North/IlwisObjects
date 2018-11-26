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

/**
  @file
  @author Stefan Frings
*/

#include "httpresponse.h"

HttpResponse::HttpResponse(QTcpSocket* socket) {
    this->socket=socket;
    statusCode=200;
    statusText="OK";
    sentHeaders=false;
    sentLastPart=false;
}

void HttpResponse::setHeader(QByteArray name, QByteArray value) {
    Q_ASSERT(sentHeaders==false);
    headers.insert(name,value);
}

void HttpResponse::setHeader(QByteArray name, int value) {
    Q_ASSERT(sentHeaders==false);
    headers.insert(name,QByteArray::number(value));
}

QMap<QByteArray,QByteArray>& HttpResponse::getHeaders() {
    return headers;
}

void HttpResponse::setStatus(int statusCode, QByteArray description) {
    this->statusCode=statusCode;
    statusText=description;
}

void HttpResponse::writeHeaders() {
    Q_ASSERT(sentHeaders==false);
    QByteArray buffer;
    buffer.append("HTTP/1.1 ");
    buffer.append(QByteArray::number(statusCode));
    buffer.append(' ');
    buffer.append(statusText);
    buffer.append("\r\n");
    foreach(QByteArray name, headers.keys()) {
        buffer.append(name);
        buffer.append(": ");
        buffer.append(headers.value(name));
        buffer.append("\r\n");
    }
    foreach(HttpCookie cookie,cookies.values()) {
        buffer.append("Set-Cookie: ");
        buffer.append(cookie.toByteArray());
        buffer.append("\r\n");
    }
    buffer.append("\r\n");
    writeToSocket(buffer);
    sentHeaders=true;
}

bool HttpResponse::writeToSocket(QByteArray data) {
    int remaining=data.size();
    char* ptr=data.data();
    while (socket->isOpen() && remaining>0) {
        // Wait until the previous buffer content is written out, otherwise it could become very large
        socket->waitForBytesWritten(-1);
        int written=socket->write(ptr,remaining);
        if (written==-1) {
          return false;
        }
        ptr+=written;
        remaining-=written;
    }
    return true;
}

void HttpResponse::write(QByteArray data, bool lastPart) {
    Q_ASSERT(sentLastPart==false);
    if (sentHeaders==false) {
        QByteArray connectionMode=headers.value("Connection");
        if (!headers.contains("Content-Length") && !headers.contains("Transfer-Encoding") && connectionMode!="close" && connectionMode!="Close") {
            if (!lastPart) {
                headers.insert("Transfer-Encoding","chunked");
            }
            else {
                headers.insert("Content-Length",QByteArray::number(data.size()));
            }
        }
        writeHeaders();
    }
    bool chunked=headers.value("Transfer-Encoding")=="chunked" || headers.value("Transfer-Encoding")=="Chunked";
    if (chunked) {
        if (data.size()>0) {
            QByteArray buffer=QByteArray::number(data.size(),16);
            buffer.append("\r\n");
            writeToSocket(buffer);
            writeToSocket(data);
            writeToSocket("\r\n");
        }
    }
    else {
        writeToSocket(data);
    }
    if (lastPart) {
        if (chunked) {
            writeToSocket("0\r\n\r\n");
        }
        else if (!headers.contains("Content-Length")) {
            socket->disconnectFromHost();
        }
        sentLastPart=true;
    }
}


bool HttpResponse::hasSentLastPart() const {
    return sentLastPart;
}


void HttpResponse::setCookie(const HttpCookie& cookie) {
    Q_ASSERT(sentHeaders==false);
    if (!cookie.getName().isEmpty()) {
        cookies.insert(cookie.getName(),cookie);
    }
}

QMap<QByteArray,HttpCookie>& HttpResponse::getCookies() {
    return cookies;
}

void HttpResponse::redirect(const QByteArray& url) {
    setStatus(303,"See Other");
    setHeader("Location",url);
    write("Redirect",true);
}

const QTcpSocket *HttpResponse::host() const
{
    return socket;
}


