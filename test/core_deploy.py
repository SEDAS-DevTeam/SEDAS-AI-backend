import tkinter as tk
import redis
import atexit

main = tk.Tk()
main.title("SEDAS core-tester")
main.geometry("500x500")

#variables
out_text = tk.StringVar()
out_voice = tk.StringVar()
out_speech = tk.StringVar()

label_query = tk.Label(main, text="Input a query:")
label_query.grid(row=0, column=0, pady=10, padx=10)

query = tk.Entry(main)
query.grid(row=0, column=1)

label_text = tk.Label(main, text="TEXT MODEL OUTPUT", borderwidth=1, bg="red")
label_voice = tk.Label(main, text="VOICE MODEL OUTPUT", borderwidth=1, bg="red")
label_speech = tk.Label(main, text="SPEECH MODEL OUTPUT", borderwidth=1, bg="red")

label_text.grid(row=2, column=0, pady=25)
label_voice.grid(row=2, column=1, pady=25)
label_speech.grid(row=2, column=2, pady=25)

label_out_voice = tk.Label(main, textvariable=out_voice)
label_out_text = tk.Label(main, textvariable=out_text)
label_out_speech = tk.Label(main, textvariable=out_speech)

label_out_voice.grid(row=3, column=0)
label_out_text.grid(row=3, column=1)
label_out_speech.grid(row=3, column=2)

main.grid_columnconfigure(0, weight=1)
main.grid_columnconfigure(1, weight=1)
main.grid_columnconfigure(2, weight=1)

#main code
main.mainloop()