/*
 * Code generated from Atmel Start.
 *
 * This file will be overwritten when reconfiguring your Atmel Start project.
 * Please copy examples or other code you want to keep to a separate file or main.c
 * to avoid loosing it when reconfiguring.
 */
#include "seal_USB.h"

#define USB_BUFFER_SIZE CONF_USB_CDCD_ACM_DATA_BULKIN_MAXPKSZ		/* Define buffer size as endpoint size */
static uint8_t single_desc_bytes[] = { CDCD_ACM_DESCES_LS_FS };		/* Device descriptors and Configuration descriptors list. */
static struct usbd_descriptors single_desc[] = { {single_desc_bytes, single_desc_bytes + sizeof(single_desc_bytes) } };	/* Make a struct of the needed descriptors */

typedef struct {
	uint8_t buff[USB_BUFFER_SIZE];		// Buffer for receiving data from host
	uint32_t head;				// Head index of the buffer
	uint32_t tail;				// tail index of the buffer
	bool outInProgress;        // indicates if there is data waiting to be read
	enum usb_xfer_code lastCode;	// transfer code of the last completed transaction
} outData_t;

typedef struct {
	uint32_t waiting;				// number of bytes waiting to be sent
	enum usb_xfer_code lastCode;	// transfer code of the last completed transaction
} inData_t;

typedef struct {
	uint8_t buff[USB_BUFFER_SIZE];		// Buffer for USB control transactions
	USB_State_t devState;		// tracks the USB device state
    bool cb_reg;               // indicates if callbacks are registered, since this must happen after EP allocation
	bool dtr;					// Flag to indicate status of DTR - Data Terminal Ready
	bool rts;					// Flag to indicate status of RTS - Request to Send
} ctrlData_t;

static volatile ctrlData_t ctrlBuf;				// CTRL endpoint buffer
static volatile outData_t  outbuf;				// OUT endpoint buffer
static volatile inData_t   inbuf;				// IN endpoint buffer

static inline int32_t usb_errConverter(int32_t err) {
    // convert USB errors to their standard error counterparts.
    switch(err) {
        case USB_BUSY           : err = ERR_BUSY;
        break;
        case USB_HALTED         : err = ERR_SUSPEND;
        break;
        case USB_ERROR          : err = ERR_FAILURE;
        break;
        case USB_ERR_DENIED     : err = ERR_DENIED;
        break;
        case USB_ERR_PARAM      : err = ERR_INVALID_ARG;
        break;
        case USB_ERR_FUNC       : err = ERR_UNSUPPORTED_OP;
        break;
        case USB_ERR_REDO       : err = ERR_NOT_INITIALIZED;
        break;
        case USB_ERR_ALLOC_FAIL : err = ERR_NO_RESOURCE;
        break;
        default: err = err;
    }
    return err;
}

/**
 * \brief Callback for USB to simply set a flag that data has been received.
 */
static bool usb_out_complete(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{
	volatile hal_atomic_t __atomic;
	atomic_enter_critical(&__atomic);

	// only modify state if the transfer was on the BULK OUT endpoint
	if(CONF_USB_CDCD_ACM_DATA_BULKOUT_EPADDR == ep) {
        outbuf.head			 = 0;
        outbuf.tail			 = count;
        outbuf.lastCode		 = rc;
        outbuf.outInProgress = false;
	}

	atomic_leave_critical(&__atomic);

	return false;		// The example code returns false on success... ?
}

/**
 * \brief Callback for USB to simply set a flag that data has been sent successfully.
 */
static bool usb_in_complete(const uint8_t ep, const enum usb_xfer_code rc, const uint32_t count)
{

	volatile hal_atomic_t __atomic;
	atomic_enter_critical(&__atomic);

	// only modify state if the transfer was on the BULK IN endpoint
	if(CONF_USB_CDCD_ACM_DATA_BULKIN_EPADDR == ep) {
        if(count > inbuf.waiting) {
            inbuf.waiting = 0;
        }
        else {
            inbuf.waiting = inbuf.waiting - count;
        }

		//inbuf.waiting  = (count > inbuf.waiting ? 0 : (inbuf.waiting - count));
		inbuf.lastCode = rc;
	}

	atomic_leave_critical(&__atomic);

	return false;		// The example code returns false on success... ?
}

/**
 * \brief Callback invoked when Line State Change
 *
 * This function is called when there is a change to the RTS and DTS control
 * lines on the serial connection.
 *
 */
static bool usb_line_state_changed(usb_cdc_control_signal_t newState)
{
	ctrlBuf.dtr      = newState.rs232.DTR;
	ctrlBuf.rts      = newState.rs232.RTS;
    ctrlBuf.devState = (USB_State_t)usbdc_get_state();

	if (cdcdf_acm_is_enabled() && !ctrlBuf.cb_reg) {
		ctrlBuf.cb_reg = true;
		/* Callbacks must be registered after endpoint allocation */
		cdcdf_acm_register_callback(CDCDF_ACM_CB_READ, (FUNC_PTR)usb_out_complete);
		cdcdf_acm_register_callback(CDCDF_ACM_CB_WRITE, (FUNC_PTR)usb_in_complete);
	}

	/* No error. */
	return false;
}

int32_t usb_start(void)
{
	int32_t err = ERR_NONE;

	// Initialize the static values to their defaults
	inbuf.waiting        = 0;
	outbuf.head          = 0;
	outbuf.tail		     = 0;
	outbuf.outInProgress = false;
    ctrlBuf.cb_reg       = false;
	ctrlBuf.dtr          = false;
	ctrlBuf.rts          = false;
	ctrlBuf.devState     = (USB_State_t)usbdc_get_state();

	/* usb stack init */
	err = usbdc_init((uint8_t*)ctrlBuf.buff);

	/* usbdc_register_funcion inside */
	err = cdcdf_acm_init();

	// start and attach the USB device
	err = usbdc_start(single_desc);
	usbdc_attach();

	err = cdcdf_acm_register_callback(CDCDF_ACM_CB_STATE_C, (FUNC_PTR)usb_line_state_changed);

	return err;
}

int32_t usb_stop(void)
{
	cdcdf_acm_stop_xfer();
	cdcdf_acm_deinit();
    usbdc_detach();
	usbdc_stop();
	usbdc_deinit();

    ctrlBuf.devState     = (USB_State_t)usbdc_get_state();
	return ERR_NONE;
}

// TODO - incorporate VBUS detection
USB_State_t usb_state(void)
{
	ctrlBuf.devState = (USB_State_t)usbdc_get_state();
	return ctrlBuf.devState;
}

bool usb_dtr(void)
{
	return ctrlBuf.dtr;
}

bool usb_rts(void)
{
	return ctrlBuf.rts;
}

// TODO - might have to manually flush the OUT buffer
void usb_haltTraffic(void) {
    cdcdf_acm_stop_xfer();
    inbuf.waiting        = 0;
    outbuf.head          = 0;
    outbuf.tail		     = 0;
    outbuf.outInProgress = false;
}

/************************ TRANSMITTING DATA *************************************/
int32_t usb_write(void* outData, uint32_t BUFFER_SIZE)
{
	int32_t err;    // error return

	// This check IS needed. cdcdf_acm_write() will drop data if bus is busy and does
	// not appear to return an error message.
	if(!ctrlBuf.dtr) {
    	err = ERR_NOT_READY;
	}
    else if(inbuf.waiting) {
        err = ERR_BUSY;
    }
    else {
		volatile hal_atomic_t __atomic;
		atomic_enter_critical(&__atomic);
		inbuf.waiting  = BUFFER_SIZE;
		atomic_leave_critical(&__atomic);

		err = cdcdf_acm_write((uint8_t*)outData, BUFFER_SIZE);
	}

	return  usb_errConverter(err);
}

int32_t usb_writeDirect(void* outData, uint32_t BUFFER_SIZE)
{
    int32_t err;

    do {
        err = cdcdf_acm_write((uint8_t*)outData, BUFFER_SIZE);
    } while(err == USB_BUSY);
    delay_ms(((BUFFER_SIZE/1000)*10)+5);

    if(err != ERR_NONE) {
        gpio_set_pin_level(LED_RED, false);
    }
    return err;
}

void usb_flushTx(void)
{
    // usb_d_ep_abort(_cdcdf_acm_funcd.func_ep_in[1]);     // out of scope :(
    inbuf.waiting        = 0;

}

/************************ RECEIVING DATA ****************************************/
int32_t usb_available()
{
    int32_t retval = ERR_NONE;       // return value, defaulted to 0

    // if the buffer is empty and a read is not in progress then request data
    if( (outbuf.tail - outbuf.head) == 0 && !outbuf.outInProgress ) {
        // request an OUT transfer. the head and tail will be adjusted in the callback function
        // this function will return negative if it fails for some reason
        retval = cdcdf_acm_read((uint8_t*)outbuf.buff, USB_BUFFER_SIZE);
    }

    // If read happened without error (or didn't happen at all) return the buffer size
    if(retval >= 0) {
        retval = (outbuf.tail - outbuf.head);
    }
    return retval;
}

int32_t usb_get(void)
{
	int32_t retval = ERR_NONE;       // return value, defaulted to 0

	retval = usb_available();
	if( retval > 0 ) {
        volatile hal_atomic_t __atomic;
        atomic_enter_critical(&__atomic);
		retval = outbuf.buff[outbuf.head++];
	    atomic_leave_critical(&__atomic);
    }
	else {
		// must return error if there are no bytes to read, 0 is a valid byte value.
		retval = ERR_FAILURE;
	}
	return retval;
}

int32_t usb_read(void* receiveBuffer, uint32_t BUFFER_SIZE)
{
	int32_t letter;         // value received from USB OUT buffer
    uint32_t i = 0;         // LCV and # of bytes received
    uint8_t* buff = (uint8_t*)receiveBuffer;    // receive buffer cast as bytes

	// Fill the provided buffer until there is is no more data or it is full
	while(i < BUFFER_SIZE) {
        letter = usb_get();
        if(letter >= 0) {
            buff[i++] = letter;
        }
		else {
			// Abort the loop if the usb OUT buffer is empty
			break;
		}
    }

	/* If there was an error, return error val. Else, return the number of bytes received. */
	return i;
}

void usb_flushRx(void)
{
	//usb_d_ep_abort(_cdcdf_acm_funcd.func_ep_out);     // out of scope :(
    outbuf.head = 0;
	outbuf.tail = 0;
}