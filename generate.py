import pandas as pd
import random
from datetime import datetime, timedelta

def generate_logs(num_records):
    levels = ["INFO", "WARNING", "ERROR", "DEBUG", "CRITICAL"]

    modules = [
        "Authentication",
        "UserManagement",
        "Database",
        "Payment",
        "API",
        "Inventory",
        "Notification",
        "Reporting",
        "Security",
        "Backup"
    ]

    messages = [
        "User login successful",
        "User logout",
        "Database connection established",
        "Database connection failed",
        "Payment transaction completed",
        "Payment transaction failed",
        "API request received",
        "API response sent",
        "Inventory updated",
        "Low stock warning",
        "Email notification sent",
        "Backup completed",
        "Backup failed",
        "Unauthorized access attempt detected",
        "Password changed",
        "New user registered",
        "Report generated",
        "Data synchronization completed",
        "Session expired",
        "System startup completed"
    ]

    start_date = datetime(2025, 1, 1)

    logs = []

    for i in range(1, num_records + 1):
        timestamp = start_date + timedelta(
            seconds=random.randint(0, 365 * 24 * 60 * 60)
        )

        logs.append({
            "log_id": i,
            "timestamp": timestamp.strftime("%Y-%m-%d %H:%M:%S"),
            "level": random.choice(levels),
            "module": random.choice(modules),
            "message": random.choice(messages)
        })

    return pd.DataFrame(logs)


# ===========================
# UBAH JUMLAH DATA DI SINI
# ===========================
jumlah_data = 100000

df = generate_logs(jumlah_data)

# Simpan ke CSV
df.to_csv("system_logs.csv", index=False)

print(f"Berhasil membuat {jumlah_data:,} data log.")
print(df.head())