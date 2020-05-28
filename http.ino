////////////////
// http setup //
////////////////
void _HttpSetup()
{
  // Http status
  httpStatus = HTTP_INIT;
}

//////////////
// http end //
//////////////
void _HttpEnd()
{
  httpServer.stop();
}
