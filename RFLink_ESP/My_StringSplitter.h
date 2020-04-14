// Version 0.3  16-03-2020, SM
//    - maximaal aantal elementen verhoogd naar 25
//
// Version 0.2  23-02-2020, SM
//    - getItemAtIndex accepts negative values (-1 is the last element)
//    - maximaal aantal elementen verhoogd naar 20
//
// Version 0.1  16-06-2019, SM
//
//  based on StringSplitter.h from Harsha C. Alva, November 26, 2016.

#ifndef My_StringSplitter_h
#define My_StringSplitter_h  0.3

#define My_StringSplitter_Max 25

// ***********************************************************************************
// ***********************************************************************************
class My_StringSplitter {

  public:

    // ***********************************************************************
    // ***********************************************************************
    My_StringSplitter ( String s, char c, unsigned int limit = My_StringSplitter_Max ){
      newString ( s, c, limit ) ;
    }
    
    // ***********************************************************************
    // ***********************************************************************
    void newString ( String s, char c, unsigned int limit = My_StringSplitter_Max ){
      count = countOccurencesOfChar ( s, c ) + 1 ;  // fix for no elements, ie return input string

      if ( count <= 1 || limit <= 1 ) {
        count = 1 ;
        op[0] = s ;
        return    ;
      }

      if ( count > limit                 ) count = limit ;
      if ( count > My_StringSplitter_Max ) count = My_StringSplitter_Max   ;

      String d = String ( c ) ;
      String first;
      String second = s;

      int current = 0;
      while ( second.indexOf(d) > -1 ) {
        if ( current >= ( count - 1 ) ) {
          //current++;
          break;
        }
        for ( int i = 0; i < second.length(); i++ ) {
          if ( second.substring ( i, i + 1 ) == d ) {
            first  = second.substring ( 0, i  ) ;
            second = second.substring ( i + 1 ) ;
            if ( first.length() > 0 )
              op [ current++ ] = first ;
            break;
          }
        }
      }
      //current = (current < MAX - 1) ? current : MAX;
      if ( second.length() > 0 )
        op [ current ] = second ;
      //else
      //  --count;
    }
    
    // ***********************************************************************
    // ***********************************************************************
    int getItemCount () {
      return count;
    }

    // ***********************************************************************
    // ***********************************************************************
    String getItemAtIndex ( int index ) {
      if ( ( index >= 0 ) && ( index < count ) )
        return op [ index ] ;
      else if ( ( index < 0 ) && ( index >= -count ) ) { 
        return op [ count + index ] ;
      }
      else
        return "" ;
    }

  // ***********************************************************************
  // ***********************************************************************
  private :
    int count = 0;
    String op [ My_StringSplitter_Max ] ;
  
    // ***********************************************************************
    // ***********************************************************************
    int countOccurencesOfChar ( String s, char c ) {
      int size = 0;
      for ( int x = 0; x < s.length(); (s[x] == c ) ? size++ : 0, x++ ) ;
      return size ;
    }
    
};
#endif