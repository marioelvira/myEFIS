////////////////
// http setup //
////////////////
void _HttpSetup(void)
{
  // Http status
  httpStatus = HTTP_INIT;
}

//////////////
// http end //
//////////////
void _HttpEnd(void)
{
  httpServer.stop();
}
