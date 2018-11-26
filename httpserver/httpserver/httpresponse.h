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

#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <QMap>
#include <QString>
#include <QTcpSocket>
#include "httpcookie.h"

#include "../httpserver_global.h"

/**
  This object represents a HTTP response, in particular the response headers.
  <p>
  Example code for proper response generation:
  <code><pre>
    response.setStatus(200,"OK"); // optional, because this is the default
    response.writeBody("Hello");
    response.writeBody("World!",true);
  </pre></code>
  <p>
  Example how to return an error:
  <code><pre>
    response.setStatus(500,"server error");
    response.write("The request cannot be processed because the servers is broken",true);
  </pre></code>
  <p>
  For performance reason, writing a single or few large packets is better than writing
  many small packets. In case of large responses (e.g. file downloads), a Content-Length
  header should be set before calling write(). Web Browsers use that information to display
  a progress bar.
*/

class HTTPSERVER_EXPORT HttpResponse {
    Q_DISABLE_COPY(HttpResponse)
public:

    /**
      Constructor.
      @param socket used to write the response
    */
    HttpResponse(QTcpSocket* socket);

    /**
      Set a HTTP response header
      @param name name of the header
      @param value value of the header
    */
    void setHeader(QByteArray name, QByteArray value);

    /**
      Set a HTTP response header
      @param name name of the header
      @param value value of the header
    */
    void setHeader(QByteArray name, int value);

    /** Get the map of HTTP response headers */
    QMap<QByteArray,QByteArray>& getHeaders();

    /** Get the map of cookies */
    QMap<QByteArray,HttpCookie>& getCookies();

    /**
      Set status code and description. The default is 200,OK.
    */
    void setStatus(int statusCode, QByteArray description=QByteArray());

    /**
      Write body data to the socket.
      <p>
      The HTTP status line and headers are sent automatically before the first
      byte of the body gets sent.
      <p>
      If the response contains only a single chunk (indicated by lastPart=true),
      the response is transferred in traditional mode with a Content-Length
      header, which is automatically added if not already set before.
      <p>
      Otherwise, each part is transferred in chunked mode.
      @param data Data bytes of the body
      @param lastPart Indicator, if this is the last part of the response.
    */
    void write(QByteArray data, bool lastPart=false);

    /**
      Indicates wheter the body has been sent completely. Used by the connection
      handler to terminate the body automatically when necessary.
    */
    bool hasSentLastPart() const;

    /**
      Set a cookie. Cookies are sent together with the headers when the first
      call to write() occurs.
    */
    void setCookie(const HttpCookie& cookie);

    /**
      Send a redirect response to the browser.
      @param url Destination URL
    */
    void redirect(const QByteArray& url);

    const QTcpSocket * host() const;

private:

    /** Request headers */
    QMap<QByteArray,QByteArray> headers;

    /** Socket for writing output */
    QTcpSocket* socket;

    /** HTTP status code*/
    int statusCode;

    /** HTTP status code description */
    QByteArray statusText;

    /** Indicator whether headers have been sent */
    bool sentHeaders;

    /** Indicator whether the body has been sent completely */
    bool sentLastPart;

    /** Cookies */
    QMap<QByteArray,HttpCookie> cookies;

    /** Write raw data to the socket. This method blocks until all bytes have been passed to the TCP buffer */
    bool writeToSocket(QByteArray data);

    /**
      Write the response HTTP status and headers to the socket.
      Calling this method is optional, because writeBody() calls
      it automatically when required.
    */
    void writeHeaders();

};

#endif // HTTPRESPONSE_H
