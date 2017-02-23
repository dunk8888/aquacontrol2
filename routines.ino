
//format bytes as KB, MB or GB with indicator
String formatBytes( size_t bytes ) {
  if ( bytes < 1024) {
    return String( bytes ) + "B";
  } else if ( bytes < ( 1024 * 1024 ) ) {
    return String( bytes / 1024.0 ) + "KB";
  } else if (bytes < ( 1024 * 1024 * 1024 ) ) {
    return String( bytes / 1024.0 / 1024.0 ) + "MB";
  } else {
    return String( bytes / 1024.0 / 1024.0 / 1024.0 ) + "GB";
  }
}

float mapFloat( double x, const double in_min, const double in_max, const double out_min, const double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

String formattedTime( const time_t t) {  ///output a time with if necessary leading zeros
  String s = String(hour(t)) + ":";
  if ( (byte)minute(t) < 10 ) {
    s += "0";
  }
  s += String(minute(t)) + ":";

  if ( (byte)second(t) < 10 ) {
    s += "0";
  }
  s += second(t);
  return s;
}

time_t localTime() {
  time_t t = now() + ( timeZone * 3600 );
  //if ( dstSet ) {
  if ( isDST( month(t), day(t), dayOfWeek(t) ) ) {
    t += 3600;
  }
  return t;
}

// IsDST(): returns true if during DST, false otherwise
boolean isDST( int mo, int dy, int dw ) {
  if ( mo < 3 || mo > 11 ) {
    return false;  // January, February, and December are out.
  }
  if ( mo > 3 && mo < 11 ) {
    return true;  // April to October are in
  }
  int previousSunday = dy - dw;
  if ( mo == 3 ) {
    return previousSunday >= 8;  // In March, we are DST if our previous Sunday was on or after the 8th.
  }
  return previousSunday <= 0; // In November we must be before the first Sunday to be DST. That means the previous Sunday must be before the 1st.
}

void setPercentage( const byte thisChannel, const time_t secondsToday ) {
  if ( !programOverride && ( secondsToday != 0 ) ) {     ///to solve flashing at midnight due to secondsToday which cant be smaller than 0 -- so at midnight there is no adjusting
    byte thisTimer = 0;
    while ( channel[thisChannel].timer[thisTimer].time < secondsToday ) {
      thisTimer++;
    }
    float dimPercentage = channel[thisChannel].timer[thisTimer].percentage - channel[thisChannel].timer[thisTimer - 1].percentage;
    time_t numberOfSecondsBetween = channel[thisChannel].timer[thisTimer].time - channel[thisChannel].timer[thisTimer - 1].time;
    time_t secondsSinceLastTimer = secondsToday - channel[thisChannel].timer[thisTimer - 1].time;
    float changePerSecond  = dimPercentage / numberOfSecondsBetween;
    channel[thisChannel].currentPercentage = channel[thisChannel].timer[thisTimer - 1].percentage + ( secondsSinceLastTimer * changePerSecond );

    //check if channel has a minimum set
    if ( channel[thisChannel].currentPercentage < channel[thisChannel].minimumLevel ) {
      channel[thisChannel].currentPercentage = channel[thisChannel].minimumLevel;
    }

    analogWrite( channel[thisChannel].pin, ( int )mapFloat( channel[thisChannel].currentPercentage, 0, 100, 0, PWMdepth ) );

    if ( channelLogging ) {
      Serial.print( F("Channel: ") );
      Serial.print( channel[thisChannel].name );
      Serial.print( F(" ") );
      Serial.print( String( channel[ thisChannel ].currentPercentage ) );
      String rawValue = String( ( int )mapFloat( channel[thisChannel].currentPercentage, 0, 100, 0, PWMdepth ) );
      Serial.print( F("% RAW: ") );
      Serial.println(  rawValue );
    }
  }
}
