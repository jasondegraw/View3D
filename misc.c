/*subfile:  misc.c  ***********************************************************/
/*                                                                            */
/*  Copyright (c) 2014 Jason W. DeGraw                                        */
/*                                                                            */
/*  This file is part of View3D.                                              */
/*                                                                            */
/*  View3D is free software: you can redistribute it and/or modify it         */
/*  under the terms of the GNU General Public License as published by         */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  View3D is distributed in the hope that it will be useful, but             */
/*  WITHOUT ANY WARRANTY; without even the implied warranty of                */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         */
/*  General Public License for more details.                                  */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with View3D.  If not, see <http://www.gnu.org/licenses/>.           */
/*                                                                            */
/******************************************************************************/
/*    utility functions   */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h> /* variable argument list macro definitions */
#include "types.h"  /* define U1, I2, etc.  */
#include "prtyp.h"  /* miscellaneous function prototypes */

extern FILE *_ulog;   /* LOG output file */
extern I1 _string[LINELEN];  /* buffer for ReadXX(); helps debugging */

#define NMAX 4        /* maximum number of calls to xxxStr at one time */
#if( __TURBOC__ )
# define BIGBUF 1     /* Control buffering in NxtWrd() and FileCopy() */
#else                 /* size of large buffer; >= 5120 to be useful */
# define BIGBUF 5120
#endif

IX _emode=1;   /* error message mode: 0=logFile, 1=DOS console, 2=Windows */



/***  error  ******************************************************************/

/*  Standard error message routine - _ulog MUST be defined.  */

IX error(IX severity, I1 *file, IX line, ...)
/* severity;  values 0 - 3 defined below
 * file;      file name: __FILE__
 * line;      line number: __LINE__
 * ...;       string variables (up to LINELEN characters total) */
{
  va_list args; /* variable argument list */
  I1* format; /* format string for vprintf */
  static IX count=0;   /* count of severe errors */
  static const I1 *head[4] = { "NOTE", "WARNING", "ERROR", "FATAL" };

  if(severity >= 0)
  {
    if(severity>3)
      severity = 3;
    else if(severity==2)
      count += 1;
    //fprintf(stderr, "%s %s (%s,%d): ", PROGRAMNAME, head[severity],
    //    file, line);
    fprintf(_ulog, "%s %s (%s,%d): ", PROGRAMSTR, head[severity],
            file, line);
    //va_start(args, format);
    va_start(args,line);
    format = va_arg(args, char*);
    //vfprintf(stderr, format, args);
    vfprintf(_ulog, format, args);
    va_end(args);
    if(severity > 2)
      exit(EXIT_FAILURE);
      //fputs("\n", stderr);
    fputs("\n", _ulog);
  }
  else if( severity < -1 )   /* clear error count */
    count = 0;

  return count;
  }  /*  end error  */

/***  sfname.c  ***************************************************************/

/*  Return pointer to file name from the full path name.  */

I1 *sfname( I1* fullpath )
  {
  I1 *name=fullpath, *c;  // allow for name == fullpath

  for( c=fullpath; *c; c++ )  // find last dir char before name
    {
    if( *c == '/' ) name = c+1;
    if( *c == '\\' ) name = c+1;
    if( *c == ':' ) name = c+1;
    }

  return name;

  }  /* end sfname */

/***  LogNote.c  *************************************************************/

/*  Note to .LOG file for non-interactive debugging.
 *  Unlike error() there is no message to the console.  */

void LogNote( I1 *file, IX line, ... )
/* file;      file name: __FILE__
 * line;      line number: __LINE__
 * ...;       string variables (up to LINELEN characters total) */
  {
  if( _ulog )
    {
    I1 message[LINELEN]; /* merged message */
    I1 name[32];         /* short file name */
    va_list argp;        /* variable argument list */
    I1 start[]=" ";      /* leading blank in message */
    I1 *msg, *s;
    IX n=1;

    msg = start;   /* merge message strings */
    s = message;
    va_start( argp, line );
    while( *msg && n<LINELEN )
      {
      while( *msg && n++<LINELEN )
        *s++ = *msg++;
      msg = (I1 *)va_arg( argp, I1 * );
      }
    *s = '\0';
    va_end( argp );

    PathSplit( file, NULL, 0, NULL, 0, name, sizeof(name), NULL, 0 );
    fprintf( _ulog, "%s (file %s, line %d)\n", message, name, line );
    fflush( _ulog );
    }

  }  /*  end LogNote  */

#if( __GNUC__ )
#include <unistd.h> // prototypes: getcwd
I1 _dirchr='/';
#elif( _MSC_VER )
#include <direct.h> // prototypes: _getcdw
I1 _dirchr='\\';
#elif( __TURBOC__ )
#include <dir.h>    // prototypes: getcwd
I1 _dirchr='\\';
#else
I1 _dirchr='\0';    // this will force a failure
#endif

/***  PathMerge.c  ***********************************************************/

/*  Merge full path from its component parts.  */

void PathMerge( I1 *fullpath, IX szfp, I1 *drv, I1 *path, I1 *name, I1 *ext )
  {
  // string indexing [ 0 | 1 | ... | sz-2 | sz-1 ] for size sz string
  IX n=0; // index to fullpath string
  I1 *c;  // pointer to source string

#if( DEBUG > 0 )
  
#endif

  if( !fullpath ) return;

  if( drv && *drv )
    for( c=drv; *c && n<szfp; c++ )
      fullpath[n++] = *c;

  if( path && *path )
    {
    for( c=path; *c && n<szfp; c++ )
      fullpath[n++] = *c;
    if( fullpath[n-1] != _dirchr && n<szfp )
      fullpath[n++] = _dirchr;  // ensure path ends with _dirchr
    }

  if( name && *name )
    for( c=name; *c && n<szfp; c++ )
      fullpath[n++] = *c;

  if( ext && *ext )
    {
    if( ext[0] != '.' && n<szfp )
      fullpath[n++] = '.';  // ensure period before extension
    for( c=ext; *c && n<szfp; c++ )
      fullpath[n++] = *c;
    }

  if( n < szfp )
    fullpath[n] = '\0';
  else
    {
    //errorc( 2, "PathMerge: fullpath overrun" );
    fullpath[szfp-1] = '\0';
    }

#if( DEBUG > 0 ) 
  if( _ulog )
    {
    fprintf( _ulog, "Merge path: %s\n", fullpath );
//  fprintf( _ulog, "   drv: %s\n", drv );
//  fprintf( _ulog, "  path: %s\n", path );
//  fprintf( _ulog, "  name: %s\n", name );
//  fprintf( _ulog, "   ext: %s\n", ext );
    fflush( _ulog );
    }
#endif
  return;

  }  /* end PathMerge */

/***  PathSplit.c  ***********************************************************/

/*  Split full path into its component parts.  Using:
 *    Visual C's _splitpath()  defined in <direct.h>
 *    Turbo C's fnsplit()  defined in <dir.h>
 *    - or - ANSI C code to do equivalent.
 *  In the call pass each part string and sizeof(string).
 *  Use NULL pointers for any parts not wanted.
 *  A null drv will leave the drive as part of the path.  */

void PathSplit( I1 *fullpath, I1 *drv, IX szd, I1 *path, IX szp,
                I1 *name, IX szn, I1 *ext, IX sze )
  {
  I1 *c, // position in fullpath
     *p; // pointer to special charactor
  IX n;  // character count

#if( DEBUG > 0 )
  //if( !fullpath ) error( 3, "PathSplit: NULL fullpath" );
#endif

  c = fullpath;
  if( drv )   // fill directory string
    {
    n = 0;
    if( fullpath[1] == ':' )
      {
      if( szd < 3 )
        ;//error( 2, "pathsplit: file drive overrun" );
      else
        {
        drv[n++] = *c++;  // copy drive characters
        drv[n++] = *c++;
        }
      }
    drv[n] =  '\0';  // terminate drive string
    }

  p = strrchr( c, _dirchr );
  if( p )  // p = last directory charactor in fullpath
    {
    if( path )
      {
      n = 0;
      while( c<=p && n<szp )
        path[n++] = *c++;
      if( n == szp )
        {  n--; }
      path[n] = '\0';  // terminate path string
      }
    c = p + 1;  // c = start of name in fullpath
    }
  
  p = strrchr( c, '.' );
  if( name )
    {
    n = 0;
    if( p )  // p = last period in fullpath
      while( c<p && n<sze )
        name[n++] = *c++;
    else     // no period in/after name
      while( *c && n<sze )
        name[n++] = *c++;
    if( n == szn )
      {  n--; }
    name[n] = '\0';  // terminate name string
    }

  if( ext )
    {
    n = 0;
    if( p )  // p = last period in fullpath
      {
      for( c=p; *c && n<sze; c++ )
        ext[n++] = *c;
      if( n == sze )
        {  n--; }
      }
    ext[n] = '\0';  // terminate extension string
    }

#if( DEBUG > 0 )
  if( _ulog )
    {
    fprintf( _ulog, "Split path: %s\n", fullpath );
    fprintf( _ulog, "   drv: %s\n", drv );
    fprintf( _ulog, "  path: %s\n", path );
    fprintf( _ulog, "  name: %s\n", name );
    fprintf( _ulog, "   ext: %s\n", ext );
    }
#endif

  }  /* end PathSplit */

/***  PathCWD.c  *************************************************************/

/*  Determine component parts of Current Working Directory.  
 *    Visual C's _getcwd()  defined in <direct.h>
 *    Turbo C's getcwd()  defined in <dir.h>
 *    These functions do not produce the trailing directory character.
 *    This function may be unique to Windows/DOS.
 *    The drive characters are not separated from the path string.
 *    This will not matter in a subsequent PathMerge( ).
 *  The getcwd functions allocate a vector at path.  */

void PathCWD( I1 *path, IX szp )
  {
#if( DEBUG > 0 )
  if( !path ) error( 3, "PathCWD: NULL path" );
#endif
#if( ANSIC )
  if( szp < 2 ) error( 3, "PathCWD: szp < 2" );
  path[0] = '.';
  path[1] = '\0';
#elif( _MSC_VER )
  if( !_getcwd( path, szp ) )
    path[0] = '\0';
#elif( __TURBOC__ || __GNUC__ )
  if( !getcwd( path, szp ) )
    path[0] = '\0';
#else
  error( 3, "PathCWD: Compiler not defined" );
#endif

  }  /* end PathCWD */

#include <limits.h> /* define: SHRT_MAX, SHRT_MIN */

/***  LongCon.c  *************************************************************/

/*  Use ANSI functions to convert a \0 terminated string to a long integer.
 *  Return 1 if string is invalid, 0 if valid.
 *  Global errno will indicate overflow.
 *  Used in place of atol() because of error processing.  */

IX LongCon( I1 *str, I4 *i )
  {
  I1 *endptr;
  I4 value;
  IX eflag=0;
#if( !__GNUC__)
  extern IX errno;
  errno = 0;
#endif

  endptr = str;
  value = strtol( str, &endptr, 0 );
  if( *endptr ) eflag = 1;
#if( !__GNUC__)
  if( errno ) eflag = 1;
#endif

  if( eflag )
    *i = 0L;
  else
    *i = value;
  return eflag;
  
  }  /* end of LongCon */

/***  IntCon.c  **************************************************************/

/*  Use ANSI functions to convert a \0 terminated string to a short integer.
 *  Return 1 if string is invalid, 0 if valid.
 *  Short integers are in the range -32767 to +32767 (INT_MIN to INT_MAX).
 *  Used in place of atoi() because of error processing.  */

IX IntCon( I1 *str, IX *i )
  {
  I4 value;    /* compute result in long integer, then chop */
  IX eflag=0;

  if( LongCon( str, &value ) ) eflag = 1;
  if( value > SHRT_MAX ) eflag = 1;
  if( value < SHRT_MIN ) eflag = 1;

  if( eflag )
    *i = 0;
  else
    *i = (IX)value;
  return eflag;
  
  }  /* end of IntCon */

/***  IntStr.c  **************************************************************/

/*  Convert an integer to a string of characters.
 *  Can handle short or long integers by conversion to long.
 *  Static variables required to retain results in calling function.
 *  NMAX allows up to NMAX calls to IntStr() in one statement. 
 *  Replaces nonstandard ITOA & LTOA functions for radix 10.  */

I1 *IntStr( I4 i )
  {
  static I1 string[NMAX][12];  // strings long enough for 32-bit integers
  static index=0;

  if( ++index == NMAX )
    index = 0;

  sprintf( string[index], "%ld", i );

  return string[index];

  }  /* end of IntStr */

#if( _MSC_VER )   /* VISUAL C version */
/***  GetKey.c  **************************************************************/
#include <conio.h>  /* prototype: _getch */

IX GetKey( void )
  {
  IX c = _getch();       /* <conio.h> read keystroke */
  if( !c )               /* function & arrow characters above 127 */
    c = 128 + _getch();

  return c;

  }  /* end GetKey */

#elif( __TURBOC__ || __WATCOMC__ )
/***  GetKey.c  **************************************************************/
#include <conio.h>  /* prototype: getch */

IX GetKey( void )
  {
  IX c = getch();        /* <conio.h> read keystroke */
  if( !c )               /* function & arrow characters above 127 */
    c = 128 + getch();

  return c;

  }  /* end GetKey */

#else
/***  GetKey.c  **************************************************************/

IX GetKey( void )
  {
  IX c = getchar();  // ANSI, requires ENTER key also

  return c;

  }  /* end GetKey */
#endif

/***  GetStr.c  **************************************************************/

/*  Get a string from the keyboard (user).  */

I1 *GetStr( I1 *prompt, I1 *str )
  {
  if( prompt[0] )
    {
    fputs( prompt, stderr );
    fputs( ": ", stderr );
    }
  if( !gets( str ) )
    error( 3, __FILE__, __LINE__, "Failed to process input");

  return str;

  }  /* end GetStr */

/***  NoYes.c  ***************************************************************/

/*  Obtain n/y response to query.  NO returns 0; YES returns 1.  */

IX NoYes( I1 *question )
  {
  IX i = -1;

  while( i == -1 )
    {
    IX response;
         /* print input prompt */
    fputs( question, stderr );
    fputs( "? (y/n)  ", stderr );

         /* get user response */
    response = GetKey();
#if( _MSC_VER || __TURBOC__ || __WATCOMC__ )
    fputc( response, stderr );
    fputc( '\n', stderr );
#endif

         /* process user response */
    switch( response )
      {
      case 'y':
      case 'Y':
      case '1':
        i = 1;
        break;
      case 'n':
      case 'N':
      case '0':
        i = 0;
        break;
      default:
        fputs( " Valid responses are:  y or Y for yes, n or N for no.\n", stderr);
      }
    }  /* end while loop */

  return i;

  }  /*  end NoYes  */

/***  Pause.c  ***************************************************************/

/*  Wait for user response before continuing.  */

void Pause( void )
  {
#if( _MSC_VER || __TURBOC__ || __WATCOMC__ )
  fputs( "Press any key to continue...", stderr );
#else
  fputs( "Press the ENTER key to continue...", stderr );
#endif

  GetKey();   /* wait for user response */

#if( _MSC_VER || __TURBOC__ || __WATCOMC__ )
  fputc( '\n', stderr );
#endif

  }  /*  end Pause  */

extern FILE *_unxt;   /* NXT input file */
extern IX _echo;      /* if true, echo NXT input file */
I1 *_nxtbuf;   /* large buffer for NXT input file */

/***  NxtOpen.c  *************************************************************/

/*  Open file_name as UNXT file.  */

void NxtOpen( I1 *file_name, I1 *file, IX line )
/* file;  source code file name: __FILE__
 * line;  line number: __LINE__ */
  {
  if( _unxt )
    error( 3, file, line, "_UNXT already open");
  _unxt = fopen( file_name, "r" );  /* = NULL if no file */
  if( !_unxt )
    error(3, file, line, "Could not open file: %s", file_name);
  }  /* end NxtOpen */

/***  NxtClose.c  ************************************************************/

/*  Close _unxt.  */

void NxtClose( void )
  {
  if( _unxt )
    {
    if( fclose( _unxt ) )
      error(2, __FILE__, __LINE__, "Problem while closing _UNXT");
    _unxt = NULL;
    }

  }  /* end NxtClose */

/***  NxtLine.c  *************************************************************/

/*  Get characters to end of line (\n --> \0); used by NxtWord().  */

I1 *NxtLine( I1 *str, IX maxlen )
  {
  IX c=0;       /* character read from _unxt */
  IX i=0;       /* current position in str */

  while( c!='\n' )
    {
    c = getc( _unxt );
    if( c==EOF ) return NULL;
    if( _echo ) putc( c, _ulog );
    if( maxlen < 1 ) continue;   // do not fill buffer
    if( c == '\r' ) continue;    // 2007/10/07 Linux EOL = \n\r
    str[i++] = (I1)c;
    if( i == maxlen )
      {
      str[i-1] = '\0';
      error(3, __FILE__, __LINE__, "Buffer overflow: %s", str);
      }
    }
  if( i )
    str[i-1] = '\0';
  else
    str[0] = '\0';

  return str;

  }  /* end NxtLine */

/***  NxtWord.c  *************************************************************/

/*  Get the next word from file _unxt.  Return NULL at end-of-file.
 *  Assuming standard word separators (blank, comma, tab),
 *  comment identifiers (! to end-of-line), and
 *  end of data (* or end-of-file). */
/*  Major change October 2007:  ContamX uses NxtWord to read multiple files
 *  which are open simultaneously. The old static variable "newl" may cause
 *  an error. It has been replaced by using ungetc() to note end-of-word (EOW)
 *  which may also be end-of-line (EOL) character.
 *  Initialization with flag = -1 in now invalid - debug checked. */

I1 *NxtWord( I1 *str, IX flag, IX maxlen )
/* str;   buffer where word is stored; return pointer.
 * flag:  0:  get next word from current position in _unxt;
          1:  get 1st word from next line of _unxt;
          2:  get remainder of current line from _unxt (\n --> \0);
          3:  get next data line from _unxt (\n --> \0);
          4:  get next line (even if comment) (\n --> \0).
 * maxlen: length of buffer to test for overflow. */
  {
  IX c;         // character read from _unxt
  IX i=0;       // current position in str
  IX done=0;    // true when start of word is found or word is complete

#ifdef _DEBUG
  if( !_unxt )
    error( 3, __FILE__, __LINE__, "_UNXT not open" );
  if( maxlen < 16 )
    error( 3, __FILE__, __LINE__, "Invalid maxlen: %d", maxlen );
#endif
  c = getc( _unxt );
  if( flag > 0 )
    {
    if( c != '\n' )  // last call did not end at EOL; ready to read next char.
      {                // would miss first char if reading first line of file.
      if( flag == 2 )
        NxtLine( str, maxlen );  // read to EOL filling buffer
      else
        NxtLine( str, 0 );  // skip to EOL
        // if flag = 1; continue to read first word on next line
      }
    if( flag > 1 )
      {
        // if flag = 2; return (partial?) line just read
      if( flag > 2 )
        {
        // if flag > 2; return all of next line (must fit in buffer)
        NxtLine( str, maxlen );
        if( flag == 3 )  // skip comment lines
          while( str[0] == '!' )
            NxtLine( str, maxlen );
#ifdef _DEBUG
        if( flag > 4 )
          error( 3, __FILE__, __LINE__, "Invalid flag: %d", flag );
#endif
        }
      ungetc( '\n', _unxt );  // restore EOL character for next call
      return str;
      }
    }
  else
    {
#ifdef _DEBUG
    if( flag < 0 )
      error( 3, __FILE__, __LINE__, "Invalid flag: %d", flag);
#endif
    if( c == ' ' || c == ',' || c == '\n' || c == '\t' )
      ; // skip EOW char saved at last call
    else
      ungetc( c, _unxt );  // restore first char of first line
    }

  while( !done )   // search for start of next word
    {
    c = getc( _unxt );
    if( c==EOF ) return NULL;
    if( _echo ) putc( c, _ulog );
    switch( c )
      {
      case ' ':          // skip word separators
      case ',':
      case '\n':
      case '\r':
      case '\t':
      case '\0':
        break;
      case '!':          // begin comment; skip to EOL
        NxtLine( str, 0 );
        break;
      case '*':          // end-of-file indicator
        NxtClose();
        return NULL;
      default:           // first character of word found
        str[i++] = (I1)c;
        done = 1;
        break;
      }
    }  // end start-of-word search

  done = 0;
  while( !done )   // search for end-of-word (EOW)
    {
    c = getc( _unxt );
    if( c==EOF ) return NULL;
    if( _echo ) putc( c, _ulog );
    switch( c )
      {
      case '\n':   // EOW characters
      case ' ':
      case ',':
      case '\t':
        str[i] = '\0';
        done = 1;
        break;
      case '\r':   // 2004/01/14 here for Linux: EOL = \n\r
      case '\0':
        break;
      default:     // accumulate word in buffer
        str[i++] = (I1)c;
        if( i == maxlen )  // with overflow test
          {
          str[i-1] = '\0';
          error(3, __FILE__, __LINE__, "Buffer overflow: %s", str);
          }
        break;
      }
    }  // end EOW search
  ungetc( c, _unxt ); // save EOW character for next call

  return str;

  }  /* end NxtWord */

#include <float.h>  /* define: FLT_MAX, FLT_MIN */

/***  ReadR8.c  **************************************************************/

R8 ReadR8( IX flag )
  {
  R8 value;

  NxtWord( _string, flag, sizeof(_string) );
  if( DblCon( _string, &value ) )
    error(2, __FILE__, __LINE__, "%s is not a (valid) number", _string);
  return value;

  }  /* end ReadR8 */

/***  ReadR4.c  **************************************************************/

/*  Convert next word from file _unxt to R4 real. */

R4 ReadR4( IX flag )
  {
  R8 value;

  NxtWord( _string, flag, sizeof(_string) );
  if( DblCon( _string, &value ) || value > FLT_MAX || value < -FLT_MAX )
    error(2, __FILE__, __LINE__, "Bad float value: %s", _string);

  return (R4)value;

  }  /* end ReadR4 */

/***  ReadIX.c  **************************************************************/

/*  Convert next word from file _unxt to IX integer. */

IX ReadIX( IX flag )
  {
  I4 value;

  NxtWord( _string, flag, sizeof(_string) );
  if( LongCon( _string, &value ) 
     || value > INT_MAX || value < INT_MIN )  // max/min depends on compiler
    error(2, __FILE__, __LINE__, "Bad integer: %s", _string);

  return (IX)value;

  }  /* end ReadIX */

/***  DblCon.c  **************************************************************/

/*  Use ANSI functions to convert a \0 terminated string to a double value.
 *  Return 1 if string is invalid, 0 if valid.
 *  Global errno will indicate overflow.
 *  Used in place of atof() because of error processing.  */

IX DblCon( I1 *str, R8 *f )
  {
  I1 *endptr;
  R8 value;
  IX eflag=0;
#if( !__GNUC__)
  extern IX errno;
  errno = 0;
#endif

  endptr = str;
  value = strtod( str, &endptr );
  if( *endptr != '\0' ) eflag = 1;
#if( !__GNUC__)
  if( errno ) eflag = 1;
#endif

  if( eflag )
    *f = 0.0;
  else
    *f = value;
  return eflag;
  
  }  /* end of DblCon */

/***  FltCon.c  **************************************************************/

/*  Use ANSI functions to convert a \0 terminated string to a float value.
 *  Return 1 if string is invalid, 0 if valid.
 *  Floats are in the range -3.4e38 to +3.4e38 (FLT_MIN to FLT_MAX).
 *  Used in place of atof() because of error processing.  */

IX FltCon( I1 *str, R4 *f )
  {
  R8 value;    /* compute result in high precision, then chop */
  IX eflag=0;

  if( DblCon( str, &value) ) eflag = 1;
  if( value > FLT_MAX ) eflag = 1;
  if( value < -FLT_MAX ) eflag = 1;

  if( eflag )
    *f = 0.0;
  else
    *f = (R4)value;
  return eflag;
  
  }  /* end of FltCon */

/***  FltStr.c  **************************************************************/

/*  Convert a float number to a string of characters;
 *  n significant digits; uses ANSI sprintf().
 *  Static string required to retain results in calling function.
 *  NMAX allows up to NMAX calls to IntStr() in one statement. 
 *  Replaces nonstandard GCVT function.  */

I1 *FltStr( R8 f, IX n )
  {
  static I1 string[NMAX][32];  /* string long enough for any practical value */
  I1 format[8];
  static IX index=0;

  if( ++index == NMAX )
    index = 0;

  sprintf( format, "%%%dg", n );
  sprintf( string[index], format, f );

  return ( string[index] );

  }  /* end of FltStr */

#include <time.h>   /* prototype: clock;  define CLOCKS_PER_SEC */

/***  CPUtime.c  *************************************************************/

/*  Determine elapsed time.  Call once to determine t1;
    call later to get elapsed time. */

R4 CPUtime( R4 t1 )
  {
  R4 t2;

  t2 = (R4)clock() / (R4)CLOCKS_PER_SEC;
  return (t2-t1);

  }  /* end CPUtime */

/***  FileOpen.c  ************************************************************/

/*  Check/open fileName. If fileName is empty or or cannot be opened, 
 *  request a name from the user.  */

FILE *FileOpen( I1 *prompt, I1 *fileName, I1 *mode, IX flag )
/*  prompt;   message to user.
 *  fileName; name of file (string _FILE_PATH long).
 *  mode;     access mode - see fopen() arguments -
 *              text files: "r" read, "w" write, "a" append;
 *            binary files: "rb" read, "wb" write, "ab" append.
 *  flag;     1 = return pointer to file; 0 = close pfile, return NULL */
  {
  FILE *pfile=NULL;
  I1 modr[4];
  IX open=1;

#ifdef _DEBUG
  I1 c=mode[0];
  IX err=0;
  if( !( c == 'r' || c == 'w' || c == 'a' ) )
    err = 1;
  c = mode[1];
  if( !( c == '\0' || c == 'b' ) )
    err = 1;
  if( ( c == 'b' && mode[2] != '\0' ) )
    err = 1;
  if( err )
    {
    error( 1, __FILE__, __LINE__, "For: ", prompt, "" );
    error( 3, __FILE__, __LINE__, "Invalid access mode: ", mode, "" );
    }
#endif

  if( mode[0] != 'r' )
    {
    strcpy( modr, mode );
    modr[0] = 'r';
    if( mode[0] == 'a' )
      fputs( "  Opening to append: ", stderr );
    else
      fputs( "  Opening to create: ", stderr );
    if( fileName[0] )
      fputs( fileName, stderr );
    fputc( '\n', stderr );
    }

  while( !pfile )
    {
    if( fileName[0] )   /* try to open file */
      {
      if( mode[0] != 'r' )  /* test write & append modes */
        {
        FILE *test = fopen( fileName, modr );
        if( mode[0] == 'w' )
          {
          if( test )
            open = NoYes( "File exists; replace" );
          }
        else
          {
          if( !test )
            open = NoYes( "File does not exist; create" );
          }
        if( test )
          fclose( test );
        }
      if( open )
        {
        pfile = fopen( fileName, mode );
        if( !pfile ) error( 2, __FILE__, __LINE__,
          "Failed to open: ", fileName, "\nTry again.\n", "" );
        }
      }
    if( !pfile )        /* ask for file name */
      GetStr( prompt, fileName );
    }

  if( flag )
    return pfile;
  else
    fclose( pfile );
  return NULL;

  }  /*  end of FileOpen  */

/***  streql.c  **************************************************************/

/*  Test for equality of two strings; return 1 if equal, 0 if not.
 *  Benchmark tests with optimized Visual C++ 7.0 indicate this 
 *  function is faster if strings differ in first 3 characters, 
 *  !strcmp( s1, s2 ) is faster otherwise.   */

IX streql( I1 *s1, I1 *s2 )
  {
  for( ; *s1; s1++, s2++ )   // not checking for s2 NULL
    if( *s1 != *s2 ) break;

  if( *s1 == *s2 )  // both must equal '\0'
    return 1;
  else              // either might equal '\0'
    return 0;

  }  /* end of streql */

