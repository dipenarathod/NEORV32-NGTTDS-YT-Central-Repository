pragma Warnings (Off);
pragma Ada_95;
pragma Source_File_Name (ada_main, Spec_File_Name => "b__yt_demo.ads");
pragma Source_File_Name (ada_main, Body_File_Name => "b__yt_demo.adb");
pragma Suppress (Overflow_Check);

package body ada_main is

   E05 : Short_Integer; pragma Import (Ada, E05, "ada__text_io_E");
   E18 : Short_Integer; pragma Import (Ada, E18, "riscv__csr_generic_E");
   E14 : Short_Integer; pragma Import (Ada, E14, "interrupts_E");
   E12 : Short_Integer; pragma Import (Ada, E12, "runtime_support_E");
   E27 : Short_Integer; pragma Import (Ada, E27, "uart0_E");


   procedure adainit is
   begin
      null;

      Ada.Text_Io'Elab_Body;
      E05 := E05 + 1;
      E18 := E18 + 1;
      Interrupts'Elab_Body;
      E14 := E14 + 1;
      E12 := E12 + 1;
      E27 := E27 + 1;
   end adainit;

   procedure Ada_Main_Program;
   pragma Import (Ada, Ada_Main_Program, "_ada_yt_demo");

   procedure main is
      Ensure_Reference : aliased System.Address := Ada_Main_Program_Name'Address;
      pragma Volatile (Ensure_Reference);

   begin
      adainit;
      Ada_Main_Program;
   end;

--  BEGIN Object file/option list
   --   /home/dipen/Downloads/neorv32-setups-main_1/neorv32-setups-main/neorv32-hal-main/yt_demo/obj/development/runtime_support.o
   --   /home/dipen/Downloads/neorv32-setups-main_1/neorv32-setups-main/neorv32-hal-main/yt_demo/obj/development/yt_demo.o
   --   -L/home/dipen/Downloads/neorv32-setups-main_1/neorv32-setups-main/neorv32-hal-main/yt_demo/obj/development/
   --   -L/home/dipen/Downloads/neorv32-setups-main_1/neorv32-setups-main/neorv32-hal-main/yt_demo/obj/development/
   --   -L/home/dipen/.local/share/alire/builds/bare_runtime_14.0.0_095db6f0/282b01b920f0d5bb2bac604ac6d9e811f26d175144bc99af963e0381e797ee94/adalib/
   --   -L/home/dipen/Downloads/neorv32-setups-main_1/neorv32-setups-main/neorv32-hal-main/lib/
   --   -static
   --   -lgnat
--  END Object file/option list   

end ada_main;
