###############################-I#####################
ugelis_include_directories(

  ${UGELIS_BASE}/subsys/usb_gm/include
  ${UGELIS_BASE}/subsys/usb_gm/host
  ${UGELIS_BASE}/subsys/usb_gm/osa
  ${UGELIS_BASE}/subsys/usb_gm/host/class

)

ugelis_sources(
      usb_host_msd.c
	  usb_host_msd_phdc.c	  
    )