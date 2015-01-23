/*****************************************************************************
*    Open LiteSpeed is an open source HTTP server.                           *
*    Copyright (C) 2013 - 2015  LiteSpeed Technologies, Inc.                 *
*                                                                            *
*    This program is free software: you can redistribute it and/or modify    *
*    it under the terms of the GNU General Public License as published by    *
*    the Free Software Foundation, either version 3 of the License, or       *
*    (at your option) any later version.                                     *
*                                                                            *
*    This program is distributed in the hope that it will be useful,         *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
*    GNU General Public License for more details.                            *
*                                                                            *
*    You should have received a copy of the GNU General Public License       *
*    along with this program. If not, see http://www.gnu.org/licenses/.      *
*****************************************************************************/
#ifndef LINKOBJPOOL_H
#define LINKOBJPOOL_H


#include <util/dlinkqueue.h>
  
template <class _Obj >
class LinkObjPool 
{
    LinkQueue   m_pool;
    int         m_chunk;
    _Obj * newObj()
    {   return new _Obj();  }

    void releaseObj( void * pObj)
    {   delete (_Obj*)pObj; }

    int allocate( int size )
    {
        while( size-- > 0 )
        {
            m_pool.push( newObj() );
        }
        return 0;
    }
    
    void release()
    {
        _Obj * pObj;
        while( ( pObj = (_Obj*)m_pool.pop() ) )
        {
            delete pObj;
        }
    }
    
public: 
    explicit LinkObjPool( int initSize = 10, int chunkSize = 10)
        : m_chunk( chunkSize )
    {
        if ( initSize )
            allocate( initSize );
    }
    
    ~LinkObjPool()
    {   release();  }
    
    _Obj * get()
    {   _Obj* pObj = (_Obj *)m_pool.pop();
        if ( !pObj )
        {
            allocate( m_chunk );
            pObj = (_Obj *)m_pool.pop();
        }
        return pObj;
    }
    
    int get( _Obj ** pObj, int n)
    {
        int i;
        for( i = 0; i < n; ++i )
        {
            *pObj = get();
            if ( !*pObj )
                break;
            ++pObj;
        }
        return i;
    }

    void recycle( LinkedObj * pObj)
    {
        if ( pObj )
        {
            m_pool.push( pObj );
        }
    }
    
    void recycle( LinkedObj ** pObj, int n)
    {
        LinkedObj ** pEnd = pObj + n;
        while( pObj < pEnd )
        {
            recycle( *pObj );
            ++pObj;
        }
    }

};


#endif
