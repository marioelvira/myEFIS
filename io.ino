//////////////
// IO setup //
//////////////
void _IOSetup()
{ 
  pinMode(DI_LED, OUTPUT);
  digitalWrite(DI_LED, DI_OFF);
  ioLed = IO_OFF;

  pinMode(DI_OUT, OUTPUT);
  digitalWrite(DI_OUT, DI_OFF);
  ioOut = IO_OFF;

  pinMode(DI_IN, INPUT);
  ioIn = IO_OFF;
}

//////////////////////
// IO state machine //
//////////////////////
void _IOLoop()
{
  if (ioLed == IO_OFF)
    digitalWrite(DI_LED, DI_OFF);
  else
    digitalWrite(DI_LED, DI_ON);

  if (ioOut == IO_OFF)
  digitalWrite(DI_OUT, DI_OFF);
  else
    digitalWrite(DI_OUT, DI_ON);

  if (digitalRead(DI_IN))
    ioIn = IO_OFF;
  else
    ioIn = IO_ON;
}
