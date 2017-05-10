import time
import datetime
import queue
import struct
import tkinter as tk

from beefi import Connect2BeeOS
from definitions import TCP_IP, TCP_PORT, BUFFER_SIZE, COMMANDS
from database import openMongoDB, storeMeasuresInDatabase


class BeeGui(tk.Frame):

    """
    __init__ : frame initialization
    ==========
    """
    def __init__(self, *args, **kwargs):
        tk.Frame.__init__(self, *args, **kwargs)

        self.buttonFrame = tk.Frame()
        self.buttonFrame.pack(side="top")

        self.textFrame = tk.Frame()
        self.textFrame.pack(side="top")

        self.promptFrame = tk.Frame()
        self.promptFrame.pack(side="top")

        self.add_button()
        self.add_text()
        self.add_prompt()

        self.s = Connect2BeeOS(TCP_IP, TCP_PORT, BUFFER_SIZE)

        self.archive = False

        self.monitoring()

    """
    add_text : add the text zone
    ==========
    """
    def add_text(self):
        self.text = tk.Text(self.textFrame, height=30, width=100, background='#111111', fg='#FFDC00')
        self.vsb = tk.Scrollbar(self.textFrame, orient="vertical", command=self.text.yview, background='#FFDC00')
        self.text.configure(yscrollcommand=self.vsb.set)
        self.text.config(state=tk.DISABLED)
        self.vsb.pack(side="right", fill="y")
        self.text.pack(side="left", fill="both", expand=True)

    """
    add_button : add the buttons
    ==========
    """
    def add_button(self):
        self.optionsButton = tk.Button(self.buttonFrame, text="Options", 
                                       command=self.open_options, state=tk.DISABLED)
        self.optionsButton.pack(side="left", expand=True)

        self.saveButton = tk.Button(self.buttonFrame, text="Save", 
                                    command=self.save_to_database)
        self.saveButton.pack(side="left", expand=True)

        self.connectButton = tk.Button(self.buttonFrame, text="Connect", 
                                       command=self.connect_to_beeos)
        self.connectButton.pack(side="left", expand=True)

        self.clearButton = tk.Button(self.buttonFrame, text="Clear", 
                                     command=self.clear_text_zone)
        self.clearButton.pack(side="left", expand=True)

    """
    add_prompt : add the prompt zone
    ==========
    """  
    def add_prompt(self):
        self.prompt = tk.Entry(self.promptFrame, width=40)
        self.prompt.pack(side="left")
        self.prompt.bind('<Return>', self.ok_to_send)

        self.sendButton = tk.Button(self.promptFrame, text="send", command=self.ok_to_send_)
        self.sendButton.pack(side="left")

    """
    monitoring : start the monitoring
    ==========
    """
    def monitoring(self):
        if self.s.isConnected() and self.data_queue.qsize() > 0:
            data = self.data_queue.get()
            data_size = len(data)
            if self.archive:
                for d in data:
                    storeMeasuresInDatabase(self.db, d, self.timestamp)
            self.write_on_text("{} - {} : {}".format(datetime.datetime.fromtimestamp(self.timestamp+data[0]['timestamp']), 
                                                     data_size, data))
        if self.s.isConnected() and self.log_queue.qsize() > 0:
            log = self.log_queue.get()
            self.write_on_text("*LOG* {}".format(log))

        self.after(10, self.monitoring)

    """
    Callbacks
    """

    """
    open_options : open option window
    ==========
    """
    def open_options(self):
        self.s.send("\006\000\000\000\000")

        response = self.response_queue.get()

        options_window = tk.Toplevel(self)
        options_window.wm_title("options")

        measure_frame = tk.Frame(options_window)
        measure_frame.pack(side="top")

        sending_frame = tk.Frame(options_window)
        sending_frame.pack(side="top")

        freqs_frame = tk.Frame(options_window)
        freqs_frame.pack(side="top")

        button_frame = tk.Frame(options_window)
        button_frame.pack(side="top")

        # measure_frame
        text_m = tk.Label(measure_frame, text="Time between measures : ")
        text_m.pack(side="left")

        spin_m = tk.Spinbox(measure_frame, width=2, from_=1, to=100)
        spin_m.delete(0, 3)
        spin_m.insert(0, response['measure'])
        spin_m.pack(side="left")

        text_m2 = tk.Label(measure_frame, text="sec")
        text_m2.pack(side="left")

        # sending_frame
        text_s = tk.Label(sending_frame, text="Packages to send : ")
        text_s.pack(side="left")

        spin_s = tk.Spinbox(sending_frame, width=2, from_=1, to=100)
        spin_s.delete(0, 3)
        spin_s.insert(0, response['sending'])
        spin_s.pack(side="left")

        # freqs_frame
        text_f = tk.Label(freqs_frame, text="Frequencies to inspect : ")
        text_f.pack(side="left")

        text_f1 = tk.Label(freqs_frame, text="f1")
        text_f1.pack(side="left")
        prompt_f1 = tk.Entry(freqs_frame, width=5)
        prompt_f1.insert(0, response['freqs'][0])
        prompt_f1.pack(side="left")

        text_f2 = tk.Label(freqs_frame, text="f2")
        text_f2.pack(side="left")
        prompt_f2 = tk.Entry(freqs_frame, width=5)
        prompt_f2.insert(0, response['freqs'][1])
        prompt_f2.pack(side="left")

        text_f3 = tk.Label(freqs_frame, text="f3")
        text_f3.pack(side="left")
        prompt_f3 = tk.Entry(freqs_frame, width=5)
        prompt_f3.insert(0, response['freqs'][2])
        prompt_f3.pack(side="left")

        def convert(x, func, bounds):
            try:
                y = int(x)
            except ValueError:
                return None

            if y < bounds[0] or y > bounds[1]: 
                return None
            else:
                return y

        def ok_clicked():
            n_measure = convert(spin_m.get(), int, [1, 100])
            n_sending = convert(spin_s.get(), int, [1, 100])
            n_freq1 = convert(prompt_f1.get(), int, [1, 500])
            n_freq2 = convert(prompt_f2.get(), int, [1, 500])
            n_freq3 = convert(prompt_f3.get(), int, [1, 500])

            error = False

            # conditions
            if n_measure == None:
                text_m.config(text="Time between measures : (t must be an int 0<t<=100 !!!) ")
                error = True

            if n_sending == None:
                text_s.config(text="Packages to send : (n must be an int, 0<n<=100 !!!) ")
                error = True

            if n_freq1 == None or n_freq2 == None or n_freq3 == None:
                text_f.config(text="Frequencies to inspect : (f must be valid int, 0<f<=500) ")
                error = True

            if not error:   
                if n_measure != response['measure']:
                    self.timestamp = time.time()
                    self.s.send(bytearray([3, n_measure, 0, 0, 0]))
                    time.sleep(0.1)
                if n_sending != response['sending']:
                    self.s.send(bytearray([4, n_sending, 0, 0, 0]))
                    time.sleep(0.1)
                if (n_freq1, n_freq2, n_freq3) != response['freqs']:
                    self.s.send(bytearray([5, int(n_freq1/2), int(n_freq2/2), int(n_freq3/2), 0]))
                    time.sleep(0.1)
                self.write_on_text(">> beeOS configuration updated")
                options_window.destroy()


        def cancel_clicked():
            options_window.destroy()

        # button_frame
        okButton = tk.Button(button_frame, text="Apply", command=ok_clicked)
        okButton.pack(side="left")

        cancelButton = tk.Button(button_frame, text="Cancel", command=cancel_clicked)
        cancelButton.pack(side="left")

    """
    save_to_database : open save window
    ==========
    """
    def save_to_database(self):
        if self.archive:
            self.saveButton.config(text='archive')
            self.archive = False
        else:
            save_window = tk.Toplevel(self)
            save_window.wm_title("archive")

            text = tk.Label(save_window, text="Enter the database name : ")
            text.pack(side="left")

            database_name = None

            def ok_clicked(event):
                database_name = prompt.get()
                if database_name.find(' ') != -1:
                    text.config(text='Enter the database name (no space) : ')
                    database_name = ''
                if len(database_name) > 0:
                    save_window.destroy()
                    self.saveButton.config(text='stop archiving')
                    self.archive = True
                    self.db = openMongoDB(database_name)
                    self.write_on_text('>> Connecting to database : {}'.format(database_name))

            def ok_clicked_():
                ok_clicked(None)

            prompt = tk.Entry(save_window, width=40)
            prompt.pack(side="left")
            prompt.bind('<Return>', ok_clicked)

            okButton = tk.Button(save_window, text="Archive", command=ok_clicked_)
            okButton.pack(side="left")

    """
    connect_to_beeos : connect to the os
    ==========
    """
    def connect_to_beeos(self):
        print(self.s.isConnected())
        if self.s.isConnected():
            self.write_on_text(">> Disconnected")
            self.s.deInit()
            self.optionsButton.config(state = tk.DISABLED)
            self.connectButton.config(text='connect')
            self.testConnect.destroy()
        else:
            self.write_on_text(">> Connecting to beeOS ...")
            self.data_queue, self.response_queue = self.s.connect()
            self.timestamp = time.time()
            self.write_on_text(">> Connected at {}".format(datetime.datetime.fromtimestamp(time.time())))
            self.connectButton.config(text='disconnect')
            self.optionsButton.config(state = tk.NORMAL)
            self.testConnect = tk.Button(self.buttonFrame, text="test connection", command=self.send_toggle)
            self.testConnect.pack(side="left", expand=True)


    """
    send_toggle : just 4 fun
    ==========
    """
    def send_toggle(self):
        for i in range(2):
            self.s.send(COMMANDS['toggle red'])
            time.sleep(0.5)
            self.s.send(COMMANDS['toggle green'])
            time.sleep(0.5)


    def clear_text_zone(self):
        self.text.config(state=tk.NORMAL)
        self.text.delete(1.0,"end")
        self.text.config(state=tk.DISABLED)


    """
    ok_to_send_ : To bypass the needed argument problem, ref: ok_to_send
    ==========
    """
    def ok_to_send_(self):
        self.ok_to_send(None)        

    """
    ok_to_send : send the prompt text
    ==========
    """
    def ok_to_send(self, event):
        command = self.prompt.get()
        self.prompt.delete(0, len(command))
        if len(command) > 0:
            if self.s.isConnected():
                if command in COMMANDS:
                    self.s.send(COMMANDS[command])
                    self.write_on_text(">> {} : OK".format(command))
                else:
                    self.write_on_text(">> {} : ERROR".format(command))
            else:
                self.write_on_text(">> Error : not connected yet ...")

    """
    onquit : stop beeOS measures before quitting
    ==========
    """
    def onquit(self):
        if self.s.isConnected():
            self.s.send([7,0,0,0,0])

    """
    Utils
    """

    """
    write_on_text : write in textzone
    ==========
    """
    def write_on_text(self, text):
        self.text.config(state=tk.NORMAL)
        self.text.insert("end", text + "\n")
        self.text.see("end")
        self.text.config(state=tk.DISABLED)

if __name__ == "__main__":
    root =tk.Tk()
    frame = BeeGui(root)
    frame.pack(fill="both", expand=True)
    root.protocol("WM_DELETE_WINDOW", frame.onquit())
    try:
        root.mainloop()
    except KeyboardInterrupt:
        frame.onquit()

