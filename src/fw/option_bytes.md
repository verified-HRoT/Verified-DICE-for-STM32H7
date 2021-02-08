Option Bytes
============

For more information on the meaning of each field, check document "RM0433",
chapter 3 "(Embedded Flash memory (FLASH))", section "Option bytes overview".

``FOPTSR_PRG``
--------------

Register: ``FLASH_OPTSR``

It can have two values:

- RDP level 1: ``0x1BF6BBF0``
- RDP level 2: ``0x1BF6CCF0``

+------------------+---------------+-------------------------------------------+
| Field            | Value         | Meaning                                   |
+------------------+---------------+-------------------------------------------+
| ``SWAP_BANK_OP`` | ``0``         | Don't swap flash banks.                   |
| ``OPTCHANGEERR`` | ``0``         | OB write error flag (0 means no error).   |
| ``IO_HSLV``      | ``0*``        | Default value.                            |
| ``Res[28]``      | ``1*``        | Default value.                            |
| ``Res[27:24]``   | ``0xB*``      | Default value.                            |
| ``Res[23:22]``   | ``0x3*``      | Default value.                            |
| ``SECURITY``     | ``1``         | Enable RSS services (Secure User Memory). |
| ``ST_RAM_SIZE``  | ``0x2*``      | Default value.                            |
| ``IWDG_FZ_SDBY`` | ``1``         | Watchdog keeps counding in Stop mode.     |
| ``IWDG_FZ_STOP`` | ``1``         | Watchdog keeps counding in Standby mode.  |
| ``Res[16]``      | ``0*``        | Default value.                            |
| ``RDP[7:0]``     | ``0xBB/0xCC`` | RDP level 1 / RDP level 2.                |
| ``NRST_STDY_D1`` | ``1*``        | Default value.                            |
| ``NRST_STOP_D1`` | ``1*``        | Default value.                            |
| ``Res[5]``       | ``1*``        | Default value.                            |
| ``IWDG1_SW``     | ``1``         | Enable IWDG by SW insted of at boot.      |
| ``BOR_LEV``      | ``0x0*``      | Default value.                            |
| ``Res[1:0]``     | ``0x0*``      | Default value.                            |
+------------------+---------------+-------------------------------------------+

Note: The values with a ``*`` mark are ignored during the protection check done
when DICE boots. The values mentioned here are the ones that are set because they
need to be set to something.

``FPRAR_PRG_A``
---------------

Register: ``FLASH_PRAR_x1``
Value: ``0x000000FF``

+----------------------+-----------+---------------------------------------------+
| Field                | Value     | Meaning                                     |
+----------------------+-----------+---------------------------------------------+
| ``DMEP1``            | ``0``     | Readout protection area in bank 1 disabled. |
| ``PROT_AREA_END1``   | ``0x000`` | Default value.                              |
| ``PROT_AREA_START1`` | ``0x0FF`` | Default value.                              |
+----------------------+-----------+---------------------------------------------+

``FPRAR_PRG_B``
---------------

Register: ``FLASH_PRAR_x2``
Value: ``0x000000FF``

+----------------------+-----------+---------------------------------------------+
| Field                | Value     | Meaning                                     |
+----------------------+-----------+---------------------------------------------+
| ``DMEP2``            | ``0``     | Readout protection area in bank 2 disabled. |
| ``PROT_AREA_END2``   | ``0x000`` | Default value.                              |
| ``PROT_AREA_START2`` | ``0x0FF`` | Default value.                              |
+----------------------+-----------+---------------------------------------------+

``FSCAR_PRG_A``
---------------

Register: ``FLASH_SCAR_x1``
Value: ``0x82030000``

+---------------------+-----------+--------------------------------------------+
| Field               | Value     | Meaning                                    |
+---------------------+-----------+--------------------------------------------+
| ``DMES1``           | ``1``     | Secure-only access area in bank 1 enabled. |
| ``SEC_AREA_END1``   | ``0x203`` | Cover DICE and key manifest.                 |
| ``SEC_AREA_START1`` | ``0x000`` | Start of flash bank 1.                     |
+---------------------+-----------+--------------------------------------------+

``FSCAR_PRG_B``
---------------

Register: ``FLASH_SCAR_x2``
Value: ``0x00000FFF``

+---------------------+-----------+---------------------------------------------+
| Field               | Value     | Meaning                                     |
+---------------------+-----------+---------------------------------------------+
| ``DMES2``           | ``0``     | Secure-only access area in bank 2 disabled. |
| ``SEC_AREA_END2``   | ``0x000`` | Disabled.                                   |
| ``SEC_AREA_START2`` | ``0xFFF`` | Disabled.                                   |
+---------------------+-----------+---------------------------------------------+

``FWPSN_PRG_A``
---------------

Register: ``FLASH_WPSN_x1``
Value: ``0x000000FF``

+-----------------+----------+----------------------------------------------+
| Field           | Value    | Meaning                                      |
+-----------------+----------+----------------------------------------------+
| ``WRPSn1[7:0]`` | ``0xFF`` | Allow writes to all sectors of flash bank 1. |
+-----------------+----------+----------------------------------------------+

``FWPSN_PRG_B``
---------------

Register: ``FLASH_WPSN_x2``
Value: ``0x000000FE``

+-----------------+----------+----------------------------------------+
| Field           | Value    | Meaning                                |
+-----------------+----------+----------------------------------------+
| ``WRPSn2[7:0]`` | ``0xFE`` | Prohibit writes to sector 1 of bank 2. |
+-----------------+----------+----------------------------------------+

``FBOOT7_PRG``
--------------

Register: ``FLASH_BOOT``
Value: ``0x08000800``

+---------------------+------------+---------------------------------------+
| Field               | Value      | Meaning                               |
+---------------------+------------+---------------------------------------+
| ``BOOT_ADD1[15:0]`` | ``0x0800`` | Boot address 1 is the start of flash. |
| ``BOOT_ADD0[15:0]`` | ``0x0800`` | Boot address 0 is the start of flash. |
+---------------------+------------+---------------------------------------+

``RFU``
-------

This entry is Reserved for Future Use. The value is ignored.

Value: ``0x00000000``
