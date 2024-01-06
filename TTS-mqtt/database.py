import sqlite3

conn = sqlite3.connect('tts_mqtt.db')
cursor = conn.cursor()

cursor.execute("DROP TABLE IF EXISTS textdata")

cursor.execute('''CREATE TABLE IF NOT EXISTS textdata
                  (id INTEGER PRIMARY KEY, text TEXT)''')

data = [
    (1, 'سلام چطوری؟'),
    (2, 'غذا آماده است'),
    (3, 'ارائه پروژه'),
    (4, 'ساعت 12'),
    (5, 'هست')
]

for record in data:
    conn = sqlite3.connect('tts_mqtt.db')
    cursor = conn.cursor()
    cursor.execute('INSERT INTO textdata VALUES (?, ?)', record)
    conn.commit()
    conn.close()