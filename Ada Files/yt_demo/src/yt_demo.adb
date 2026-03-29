with Runtime_Support; use Runtime_Support;

with neorv32.UART0;
with neorv32;
with Uart0;

with Ada.Text_IO; use Ada.Text_IO;

procedure Yt_Demo is
begin
   Uart0.Init(19200);
   Put_Line ("Hellooooooooooooooooo");
   null;
end Yt_Demo;
