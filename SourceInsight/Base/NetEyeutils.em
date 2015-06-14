/* NetEyeUtils.em - a small collection of useful editing macros */

/*Save system's information*/
macro SaveSysInfo( )
{
    Name    = Ask( "Please enter your name:" )
    Version = Ask( "Version:" )

    hSysBuf = NewBuf( "SystemInfo" )
    if( hSysBuf == hNil ){
        return 1
    }

    AppendBufLine( hSysBuf, Name )//line 0
    AppendBufLine( hSysBuf, Version )//line 1

    SaveBufAs( hSysBuf, "system.ini" )
}

macro AddModInfo()
{
    hSysBuf = OpenBuf( "system.ini" )
    if( hSysBuf == hNil ){
        return 1
    }

    MyName  = GetBufLine( hSysBuf, 0 )
    Version = GetBufLine( hSysBuf, 1 )

    SysTime = GetSysTime( 0 )

    year  = SysTime.Year
    month = SysTime.Month
    day   = SysTime.Day
    hour  = SysTime.Hour + 8
    minute= SysTime.Minute

    /*
    j = 0
    counter = 0
    head = 0
    tail = 0    
    while( 1 ){
        if( AsciiFromChar( SysTime[j] ) == 34 ){
            counter = counter + 1
        }

        if( counter == 1 && head == 0 ){
            head = j
        }
        if( counter == 2 && tail == 0 ){
            tail = j
            break;
        }
        
        j = j + 1       
    }
    
    date    = StrMid( SysTime, head + 1, tail )
    */

    hwnd    = GetCurrentWnd()
    lnFirst = GetWndSelLnFirst( hwnd )
    lnLast  = GetWndSelLnLast( hwnd )

    hbuf = GetCurrentBuf()

    //modified on 2003.12.10
    /*if( lnFirst != lnLast )*/{
        firstBuf = ""
        lastBuf  = ""
        spaceBuf = ""
        referencedBuf = GetBufLine( hbuf, lnFirst )

        i = 0
        while( referencedBuf[i] == " " || referencedBuf[i] == "\t" ){
            if( referencedBuf[i] == " " ){
                spaceBuf = cat( spaceBuf, " " )//space
            }else{
                spaceBuf = cat( spaceBuf, "\t" )//Tab
            }
            i = i + 1
        }

       // firstBuf = cat( spaceBuf, "//Start of @MyName@ on @year@-@month@-@day@ @hour@:@minute@ @Version@" )
        //lastBuf  = cat( spaceBuf, "//End of @MyName@ on @year@-@month@-@day@ @hour@:@minute@ @Version@" )
        firstBuf = cat( spaceBuf, "/*--Start of @MyName@ on @year@-@month@-@day@ @hour@:@minute@ --*/" )
        lastBuf  = cat( spaceBuf, "/*--End of @MyName@ on @year@-@month@-@day@ @hour@:@minute@ --*/" )
        InsBufLine( hbuf, lnFirst, firstBuf )
        InsBufLine( hbuf, lnLast + 2, lastBuf )
    }/*else{
        curText = GetBufLine( hbuf, lnFirst )*/
        //newText = cat( curText, "/*Modifed by @MyName@ on @date@ @Version@*/" )
        /*DelBufLine( hbuf, lnFirst )
        InsBufLine( hbuf, lnFirst, newText )
    }*/

    CloseBuf( hSysBuf )
}

macro AddRemarkInfo()
{
            hSysBuf = OpenBuf( "system.ini" )
    if( hSysBuf == hNil ){
        return 1
    }
    
    szMyName  = GetBufLine( hSysBuf, 0 )
    SysTime = GetSysTime( 0 )
    year  = SysTime.Year
    month = SysTime.Month
    day   = SysTime.Day
    
    hwnd    = GetCurrentWnd()
    lnFirst = GetWndSelLnFirst( hwnd )
    lnLast  = GetWndSelLnLast( hwnd )

    hbuf = GetCurrentBuf()

    curFirstText = GetBufLine( hbuf, lnFirst )

    i = 0
    tmpFirstText = ""
    while( curFirstText[i] == " " || curFirstText[i] == "\t" ){
        if( curFirstText[i] == " " ){
            tmpFirstText = cat( tmpFirstText, " " )
        }else{
            tmpFirstText = cat( tmpFirstText, "\t" )
        }
        i = i + 1
    }

    len = strlen( curFirstText )

    newFirstText = strmid( curFirstText, i, len )

    if( lnFirst == lnLast ){
        /*
        GetWndSelIchFirst (hwnd)
        GetWndSelIchLim (hwnd)
        */
        //modified on 2003.12.10
        tmpFirstText = cat( tmpFirstText, "/*" )
        newFirstText = cat( tmpFirstText, newFirstText )
        newFirstText = cat( newFirstText, "*///@szMyName@ @year@-@month@-@day@" )

        DelBufLine( hbuf, lnFirst )
        InsBufLine( hbuf, lnFirst, newFirstText )
    }else{
        tmpFirstText = cat( tmpFirstText, "/*" )
        newFirstText = cat( tmpFirstText, newFirstText )

        curLastText  = GetBufLine( hbuf, lnLast )
        newLastText  = cat( curLastText, "*/ //@szMyName@ @year@-@month@-@day@" )

        DelBufLine( hbuf, lnFirst )
        InsBufLine( hbuf, lnFirst, newFirstText )
        DelBufLine( hbuf, lnLast )
        InsBufLine( hbuf, lnLast, newLastText )
    }
}

macro AddCommentInfo()
{
    hSysBuf = OpenBuf( "system.ini" )
    if( hSysBuf == hNil ){
        return 1
    }
    
    szMyName  = GetBufLine( hSysBuf, 0 )
    SysTime = GetSysTime( 0 )
    year  = SysTime.Year
    month = SysTime.Month
    day   = SysTime.Day
    
    hwnd    = GetCurrentWnd()
    lnFirst = GetWndSelLnFirst( hwnd )
    lnLast  = GetWndSelLnLast( hwnd )

    hbuf = GetCurrentBuf()

    curFirstText = GetBufLine( hbuf, lnFirst )

    i = 0
    tmpFirstText = ""
    while( curFirstText[i] == " " || curFirstText[i] == "\t" ){
        if( curFirstText[i] == " " ){
            tmpFirstText = cat( tmpFirstText, " " )
        }else{
            tmpFirstText = cat( tmpFirstText, "\t" )
        }
        i = i + 1
    }

    len = strlen( curFirstText )

    newFirstText = strmid( curFirstText, i, len )

    if( lnFirst == lnLast ){
        /*
        GetWndSelIchFirst (hwnd)
        GetWndSelIchLim (hwnd)
        */
        //modified on 2003.12.10
        //tmpFirstText = cat( tmpFirstText, "/*" )
        newFirstText = cat( tmpFirstText, newFirstText )
        newFirstText = cat( newFirstText, "//@szMyName@ @year@-@month@-@day@" )

        DelBufLine( hbuf, lnFirst )
        InsBufLine( hbuf, lnFirst, newFirstText )
    }else{
       // tmpFirstText = cat( tmpFirstText, "" )
        newFirstText = cat( tmpFirstText, newFirstText )

        curLastText  = GetBufLine( hbuf, lnLast )
        newLastText  = cat( curLastText, "//@szMyName@ @year@-@month@-@day@" )

        DelBufLine( hbuf, lnFirst )
        InsBufLine( hbuf, lnFirst, newFirstText )
        DelBufLine( hbuf, lnLast )
        InsBufLine( hbuf, lnLast, newLastText )
    }
}

macro AddIf0Identifier()
{
        hSysBuf = OpenBuf( "system.ini" )
    if( hSysBuf == hNil ){
        return 1
    }
    
    szMyName  = GetBufLine( hSysBuf, 0 )
    SysTime = GetSysTime( 0 )
    year  = SysTime.Year
    month = SysTime.Month
    day   = SysTime.Day

    hwnd    = GetCurrentWnd()
    lnFirst = GetWndSelLnFirst( hwnd )
    lnLast  = GetWndSelLnLast( hwnd )

    hbuf = GetCurrentBuf()

    curFirstText = GetBufLine( hbuf, lnFirst )

    if( strlen( curFirstText) <= 0 ){
        return 1
    }   

    i = 0
    tmpFirstText = ""
    while( curFirstText[i] == " " || curFirstText[i] == "\t" ){
        if( curFirstText[i] == " " ){
            tmpFirstText = cat( tmpFirstText, " " )
        }else{
            tmpFirstText = cat( tmpFirstText, "\t" )
        }
        i = i + 1
    }

    if( lnFirst <= 1 ){
        return 1
    }

    newText = cat( tmpFirstText, "#if 0 /*@szMyName@ @year@-@month@-@day@*/" )
    InsBufLine( hbuf, lnFirst, newText )
    newText = cat( tmpFirstText, "#endif" )
    InsBufLine( hbuf, lnLast + 2, newText )
}

/*-------------------------------------------------------------------------
    I N S E R T   H E A D E R

    Inserts a comment header block at the top of the current function. 
    This actually works on any type of symbol, not just functions.

    To use this, define an environment variable "MYNAME" and set it
    to your email name.  eg. set MYNAME=raygr
-------------------------------------------------------------------------*/
macro AddFuncHeader()
{
    hSysBuf = OpenBuf( "system.ini" )
    if( hSysBuf == hNil ){
        return 1
    }

    //szVersion = GetBufLine( hSysBuf, 1 )

    //SysTime = GetSysTime( 0 )
    //date    = StrMid( SysTime, 6, 19 )

    szMyName  = GetBufLine( hSysBuf, 0 )
    //Version = GetBufLine( hSysBuf, 1 )

    SysTime = GetSysTime( 0 )
    year  = SysTime.Year
    month = SysTime.Month
    day   = SysTime.Day
    
    // Get a handle to the current file buffer and the name
    // and location of the current symbol where the cursor is.
    hbuf = GetCurrentBuf()

    if( hbuf == hNil ){
        return 1
    }

    ln = GetBufLnCur( hbuf )

    // begin assembling the title string
    
    //InsBufLine(hbuf, ln+1, "/*-------------------------------------------------------------------------*")
    
    /* if owner variable exists, insert Owner: name */
    /*if (strlen(szMyName) > 0){
        InsBufLine( hbuf, ln + 2, "Function:" )
        InsBufLine( hbuf, ln + 3, "Created By:   @szMyName@" )
        InsBufLine( hbuf, ln + 4, "Created Date: @date@" )
        InsBufLine( hbuf, ln + 5, "Modified By:" )
        InsBufLine( hbuf, ln + 6, "Modified Date:" )
        InsBufLine( hbuf, ln + 7, "Parameters:" )
        InsBufLine( hbuf, ln + 8, "Returns:" )
        InsBufLine( hbuf, ln + 9, "Calls:" )
        InsBufLine( hbuf, ln + 10, "Called By:" )
        ln = ln + 10
    }else{
        ln = ln + 2
    }*/

    lnStartPos = ln + 2;
    if (strlen(szMyName) > 0){
    /*
        InsBufLine( hbuf, ln + 1, "/*******************************************************************" )
        InsBufLine( hbuf, ln + 2, " 作    者:@szMyName@" )
        InsBufLine( hbuf, ln + 3, " 版    权: 新国都. " )
        InsBufLine( hbuf, ln + 4, " 函数名称:" )
        InsBufLine( hbuf, ln + 5, " 函数功能:" )
        InsBufLine( hbuf, ln + 6, " 入口参数:" )
        InsBufLine( hbuf, ln + 7, " 返 回 值:" )
        InsBufLine( hbuf, ln + 8, " 相关调用:" )
        InsBufLine( hbuf, ln + 9, " 备    注:" )
        InsBufLine( hbuf, ln + 10, "创建信息: @year@-@month@-@day@" )
        InsBufLine( hbuf, ln + 11, " 修改信息:" )
       */
        InsBufLine( hbuf, ln + 1, "/****************************************************************************" )
        InsBufLine( hbuf, ln + 2, "**Description:           " )
        InsBufLine( hbuf, ln + 3, "**Input parameters:  " )
        InsBufLine( hbuf, ln + 4, "**Output parameters: " )
        InsBufLine( hbuf, ln + 5, "**                   " )
        InsBufLine( hbuf, ln + 6, "**Returned value:        " )
        InsBufLine( hbuf, ln + 7, "**                   " )
        InsBufLine( hbuf, ln + 8, "**Created by:            @szMyName@(@year@-@month@-@day@)" )
        InsBufLine( hbuf, ln + 9, "**--------------------------------------------------------------------------" )
        InsBufLine( hbuf, ln + 10, "**Modified by:      " )
        InsBufLine( hbuf, ln + 11, "**Modified by:      " )
        ln = ln + 11
    }else{
        ln = ln + 2
    }

    //InsBufLine(hbuf, ln+1, "*-------------------------------------------------------------------------*/")
    InsBufLine( hbuf, ln + 1,     "****************************************************************************/" )
    
    // put the insertion point inside the header comment
    SetBufIns( hbuf, lnStartPos, 20 )

    CloseBuf( hSysBuf )
}


/* InsertFileHeader:

   Inserts a comment header block at the top of the current function. 
   This actually works on any type of symbol, not just functions.

   To use this, define an environment variable "MYNAME" and set it
   to your email name.  eg. set MYNAME=raygr
*/

macro AddFileHeader()
{
    hSysBuf = OpenBuf( "system.ini" )
    if( hSysBuf == hNil ){
        return 1
    }

    szMyName  = GetBufLine( hSysBuf, 0 )
    Version = GetBufLine( hSysBuf, 1 )

    SysTime = GetSysTime( 0 )
    fileNameBuf = GetFileName(20)
    //modified on 2003.12.10
    //date    = StrMid( SysTime, 6, 19 )
    year  = SysTime.Year
    month = SysTime.Month
    day   = SysTime.Day
    
    hbuf = GetCurrentBuf()

    //InsBufLine( hbuf, 0, "/*=========================================================================*" )
    //InsBufLine( hbuf, 0, "/************************************************************" )
    
    /* if owner variable exists, insert Owner: name */
    /*InsBufLine( hbuf, 1, "FileName:" )
    InsBufLine( hbuf, 2, "Created by:@szMyName@" )
    InsBufLine( hbuf, 3, "Created date:@date@" )
    InsBufLine( hbuf, 4, "Content:" )
    InsBufLine( hbuf, 5, "Modified records:" )
    InsBufLine( hbuf, 6, "1. YYYY.MM.DD:Mr Bill Gates add ......" )
    InsBufLine( hbuf, 7, "    Mr. Bill Gates add ......" )*/
    
    //InsBufLine( hbuf, 1, "/************************************************************" )
    InsBufLine( hbuf, 0, "/********************************Copyright ( c ) ***************************" )
    InsBufLine( hbuf, 1, "** File     :                  " )
    InsBufLine( hbuf, 2, "** Abstruct :                  " )
    InsBufLine( hbuf, 3, "** " )
    InsBufLine( hbuf, 4, "** " )
    InsBufLine( hbuf, 5, "**Created By:     @szMyName@" )
    InsBufLine( hbuf, 6, "**Created Date:       @year@-@month@-@day@" )
    InsBufLine( hbuf, 7, "**Version:            <version >" )
    InsBufLine( hbuf, 8, "**Description:    " )
    InsBufLine( hbuf, 9, "**" )
    InsBufLine( hbuf, 10, "****************************************************************************/" )

    SetBufIns( hbuf, 1, 19 )
    
    //InsBufLine( hbuf, 8, "*=========================================================================*/" )

    CloseBuf( hSysBuf )
}
