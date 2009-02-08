/*
 * Copyright (c) 2009 Matthew Jeffries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * $Id$
 */

#ifndef PHP_AX_GAMESERVER_QUERY_H
#define PHP_AX_GAMESERVER_QUERY_H

extern zend_module_entry ax_gameserver_query_module_entry;
#define phpext_ax_gameserver_query_ptr &ax_gameserver_query_module_entry

#ifdef PHP_WIN32
#define PHP_AX_GAMESERVER_QUERY_API __declspec(dllexport)
#else
#define PHP_AX_GAMESERVER_QUERY_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(ax_gameserver_query);
PHP_MSHUTDOWN_FUNCTION(ax_gameserver_query);
PHP_RINIT_FUNCTION(ax_gameserver_query);
PHP_RSHUTDOWN_FUNCTION(ax_gameserver_query);
PHP_MINFO_FUNCTION(ax_gameserver_query);

PHP_FUNCTION(axgsq_version);
PHP_FUNCTION(axgsq_connect);
PHP_FUNCTION(axgsq_disconnect);
PHP_FUNCTION(axgsq_get_serverinfo);

#ifdef ZTS
#define AX_GAMESERVER_QUERY_G(v) TSRMG(ax_gameserver_query_globals_id, zend_ax_gameserver_query_globals *, v)
#else
#define AX_GAMESERVER_QUERY_G(v) (ax_gameserver_query_globals.v)
#endif

#endif	/* PHP_AX_GAMESERVER_QUERY_H */
