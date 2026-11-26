# Algoritma ve Programlama — Grup Ödevi: CPU Süreç Zamanlayıcı Simülasyonu (Abdullah, Eren, Taha)

Tek çekirdekli bir CPU zamanlayıcısının **FCFS (First Come First Served)**, **SJF (Shortest Job First - Kesintisiz)** ve **Round Robin (Tur Tabanlı)** algoritmalarıyla C dilinde simülasyonu.

## 👥 Grup Üyeleri ve Görev Dağılımı

| Grup Üyesi | GitHub | Sorumlu Olduğu Algoritma ve Fonksiyonlar |
| :--- | :--- | :--- |
| **Abdullah Arslan** | [@1abdullaharslan](https://github.com/1abdullaharslan) | **FCFS Algoritması**, Veri Girişi (`surecleri_oku`), Sistem Sıfırlama (`sistemi_sifirla`), Paralel Dizi Sıralaması (`paralel_dizi_sirala`, `fcfs_calistir`) |
| **Taha Yasin Karateke** | [@tahayasinkarateke](https://github.com/tahayasinkarateke) | **SJF Algoritması**, Gantt Şeması ve Bağlam Değişimi (`gantt_ekle`, `gantt_yazdir`), En Kısa İşi Bulma (`en_kisa_isi_bul`, `sjf_calistir`) |
| **Muammer Eren Özünlü** | [@merenozunlu](https://github.com/merenozunlu) | **Round Robin Algoritması** (`round_robin_calistir`), Süre Hesaplamaları (`sureleri_hesapla`), Pointer ile Ortalamalar (`ortalamalar`), Sonuç Tablosu (`sonuc_tablosu_yazdir`) |
| **Ortak Çalışma** | Tüm Grup | `main` fonksiyonu ve `algoritmalari_karsilastir` fonksiyonu |

##  /Kısıtlamalar ve Teknik Özellikler
- Yalnızca `<stdio.h>` kütüphanesi kullanılmıştır.
- `struct`, `malloc`/`free` kullanılmamış, süreç verileri **paralel dizilerle** yönetilmiştir.
- Fonksiyonlardan çoklu değer döndürmek için **adresle aktarım (pointer parametreler)** kullanılmıştır.
- Bağlam değişimi (context switch) takibinde `static` lokal değişken yapısı kullanılmıştır.

##  /Derleme ve Çalıştırma
```bash
gcc main.c -o simulator
./simulator
```
