#include <stdio.h>

#define MAKS_SUREC 50
#define MAKS_GANTT 500

/* 
 * İzin verilen Global Değişkenler:
 * Simülasyon saati, bağlam değişimi sayacı ve Gantt şeması kayıt dizileri.
 */
int saat = 0;
int baglam_degisimi = 0;

int gantt_surec[MAKS_GANTT];
int gantt_baslangic[MAKS_GANTT];
int gantt_bitis[MAKS_GANTT];
int gantt_sayisi = 0;

/* =========================================================================
 * BÖLÜM 1: TAHA YASİN KARATEKE'NİN FONKSİYONLARI (Gantt, Bağlam Değişimi, SJF)
 * ========================================================================= */

/*
 * Yazan: Taha Yasin Karateke
 * Açıklama: Gantt şemasına yeni bir çalışma veya boşta (BOS) bloğu ekler.
 * static lokal değişken (son_calisan_surec) kullanarak bir önceki çalışan gerçek
 * süreci hafızada tutar. Eğer yeni çalışan süreç bir öncekinden farklıysa
 * baglam_degisimi sayacını 1 artırır. surec_no == -1 gönderilirse static değişkeni sıfırlar.
 */
void gantt_ekle(int surec_no, int baslangic, int bitis) {
    static int son_calisan_surec = 0;

    /* Sistemi sıfırlarken static lokal değişkeni de sıfırlamak için özel çağrı */
    if (surec_no == -1) {
        son_calisan_surec = 0;
        return;
    }

    if (baslangic == bitis) {
        return;
    }

    /* Eğer aynı süreç kesintisiz devam ediyorsa mevcut Gantt bloğunun bitişini uzat */
    if (gantt_sayisi > 0 && gantt_surec[gantt_sayisi - 1] == surec_no) {
        gantt_bitis[gantt_sayisi - 1] = bitis;
    } else {
        gantt_surec[gantt_sayisi] = surec_no;
        gantt_baslangic[gantt_sayisi] = baslangic;
        gantt_bitis[gantt_sayisi] = bitis;
        gantt_sayisi++;
    }

    /* Bağlam değişimi kontrolü: İlk süreç ve BOS (0) blokları sayılmaz */
    if (surec_no > 0) {
        if (son_calisan_surec != 0 && son_calisan_surec != surec_no) {
            baglam_degisimi++;
        }
        son_calisan_surec = surec_no;
    }
}

/*
 * Yazan: Taha Yasin Karateke
 * Açıklama: Kaydedilen Gantt şeması bloklarını [P1 0-5] veya [BOS 18-20] biçiminde
 * ekrana yazdırır. Her satırda en fazla 6 blok bulunur.
 */
void gantt_yazdir(void) {
    int i;
    printf("\nGantt Semasi:\n");
    for (i = 0; i < gantt_sayisi; i++) {
        if (gantt_surec[i] == 0) {
            printf("[BOS %d-%d] ", gantt_baslangic[i], gantt_bitis[i]);
        } else {
            printf("[P%d %d-%d] ", gantt_surec[i], gantt_baslangic[i], gantt_bitis[i]);
        }

        /* Her satırda en fazla 6 blok kuralı */
        if ((i + 1) % 6 == 0 || i == gantt_sayisi - 1) {
            printf("\n");
        }
    }
}

/*
 * Yazan: Taha Yasin Karateke
 * Açıklama: SJF algoritması için o anki saate kadar gelmiş ve henüz tamamlanmamış
 * süreçler arasından CPU süresi en kısa olanın indeksini bulur.
 * Eşitlik durumunda geliş zamanı erken olan, o da eşitse süreç numarası küçük olan seçilir.
 */
int en_kisa_isi_bul(int n, int no[], int gelis[], int cpu[], int tamamlandi[]) {
    int i;
    int secilen = -1;

    for (i = 0; i < n; i++) {
        if (tamamlandi[i] == 0 && gelis[i] <= saat) {
            if (secilen == -1) {
                secilen = i;
            } else if (cpu[i] < cpu[secilen]) {
                secilen = i;
            } else if (cpu[i] == cpu[secilen]) {
                if (gelis[i] < gelis[secilen]) {
                    secilen = i;
                } else if (gelis[i] == gelis[secilen] && no[i] < no[secilen]) {
                    secilen = i;
                }
            }
        }
    }
    return secilen;
}

/*
 * Yazan: Taha Yasin Karateke
 * Açıklama: Kesintisiz SJF (Shortest Job First) algoritmasını simüle eder.
 * CPU her boşaldığında en_kisa_isi_bul fonksiyonu ile en kısa süreci seçip sonuna kadar çalıştırır.
 * Hiçbir süreç gelmemişse saati en erken gelecek sürece ilerletip [BOS] bloğu ekler.
 */
void sjf_calistir(int n, int no[], int gelis[], int cpu[], int bitis[]) {
    int i;
    int tamamlandi[MAKS_SUREC];
    int biten_sayisi = 0;

    for (i = 0; i < n; i++) {
        tamamlandi[i] = 0;
    }

    while (biten_sayisi < n) {
        int secilen = en_kisa_isi_bul(n, no, gelis, cpu, tamamlandi);

        if (secilen == -1) {
            /* Gelmiş süreç yoksa, bitmemiş süreçlerin en erken geliş zamanını bul */
            int en_erken = -1;
            for (i = 0; i < n; i++) {
                if (tamamlandi[i] == 0) {
                    if (en_erken == -1 || gelis[i] < en_erken) {
                        en_erken = gelis[i];
                    }
                }
            }
            gantt_ekle(0, saat, en_erken);
            saat = en_erken;
        } else {
            int baslangic = saat;
            saat += cpu[secilen];
            bitis[secilen] = saat;
            tamamlandi[secilen] = 1;
            biten_sayisi++;
            gantt_ekle(no[secilen], baslangic, saat);
        }
    }
}

/* =========================================================================
 * BÖLÜM 2: ABDULLAH ARSLAN'IN FONKSİYONLARI (Girdi, Sıfırlama, Paralel Dizi, FCFS)
 * ========================================================================= */

/*
 * Yazan: Abdullah Arslan
 * Açıklama: Kullanıcıdan süreç sayısını, her sürecin geliş zamanını, CPU süresini
 * ve Round Robin için zaman dilimi (quantum) değerini okur.
 */
void surecleri_oku(int *n, int no[], int gelis[], int cpu[], int *zaman_dilimi) {
    int i;
    printf("Surec sayisini giriniz: ");
    scanf("%d", n);

    for (i = 0; i < *n; i++) {
        no[i] = i + 1;
        printf("P%d Gelis Zamani ve CPU Suresini giriniz (orn: 0 5): ", no[i]);
        scanf("%d %d", &gelis[i], &cpu[i]);
    }

    printf("Round Robin icin Zaman Dilimi (Quantum) giriniz: ");
    scanf("%d", zaman_dilimi);
}

/*
 * Yazan: Abdullah Arslan
 * Açıklama: Her algoritmadan önce sistemi, global değişkenleri, Gantt sayaçlarını,
 * static son çalışan süreç bilgisini ve sonuç dizilerini sıfırlar.
 */
void sistemi_sifirla(int n, int bitis[], int donus[], int bekleme[]) {
    int i;
    saat = 0;
    baglam_degisimi = 0;
    gantt_sayisi = 0;
    gantt_ekle(-1, 0, 0); /* static lokal değişkeni sıfırlar */

    for (i = 0; i < n; i++) {
        bitis[i] = 0;
        donus[i] = 0;
        bekleme[i] = 0;
    }
}

/*
 * Yazan: Abdullah Arslan
 * Açıklama: Paralel dizilerde veri tutarlılığını koruyarak süreçleri verilen kriterlere göre sıralar.
 * mod == 1 ise FCFS için (önce geliş zamanı, eşitse süreç numarası) sıralar.
 * mod == 0 ise süreçleri orijinal süreç numarası (P1, P2, ..., Pn) sırasına geri döndürür.
 */
void paralel_dizi_sirala(int n, int no[], int gelis[], int cpu[], int bitis[], int mod) {
    int i, j, gecici;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            int yer_degistir = 0;
            if (mod == 1) {
                /* FCFS sıralaması: Geliş zamanı küçük olan, eşitse numarası küçük olan */
                if (gelis[j] > gelis[j + 1] ||
                    (gelis[j] == gelis[j + 1] && no[j] > no[j + 1])) {
                    yer_degistir = 1;
                }
            } else {
                /* Orijinal sıra: Süreç numarası küçük olan */
                if (no[j] > no[j + 1]) {
                    yer_degistir = 1;
                }
            }

            if (yer_degistir) {
                /* Paralel dizilerin tümünde aynı indeksleri takas et (veri tutarlılığı) */
                gecici = no[j]; no[j] = no[j + 1]; no[j + 1] = gecici;
                gecici = gelis[j]; gelis[j] = gelis[j + 1]; gelis[j + 1] = gecici;
                gecici = cpu[j]; cpu[j] = cpu[j + 1]; cpu[j + 1] = gecici;
                gecici = bitis[j]; bitis[j] = bitis[j + 1]; bitis[j + 1] = gecici;
            }
        }
    }
}

/*
 * Yazan: Abdullah Arslan
 * Açıklama: FCFS (First Come First Served) algoritmasını simüle eder.
 * Süreçleri geliş zamanına göre sıralar, kesintisiz çalıştırır ve bitince
 * dizileri tekrar süreç numarası sırasına (P1..Pn) döndürür.
 */
void fcfs_calistir(int n, int no[], int gelis[], int cpu[], int bitis[]) {
    int i;

    /* Süreçleri geliş zamanına göre sırala */
    paralel_dizi_sirala(n, no, gelis, cpu, bitis, 1);

    for (i = 0; i < n; i++) {
        /* Süreç henüz gelmediyse CPU boşta kalır */
        if (saat < gelis[i]) {
            gantt_ekle(0, saat, gelis[i]);
            saat = gelis[i];
        }

        gantt_ekle(no[i], saat, saat + cpu[i]);
        saat += cpu[i];
        bitis[i] = saat;
    }

    /* Diğer algoritmaların ve tablonun doğru çalışması için P1..Pn sırasına geri döndür */
    paralel_dizi_sirala(n, no, gelis, cpu, bitis, 0);
}

/* =========================================================================
 * BÖLÜM 3: MUAMMER EREN ÖZÜNLÜ'NÜN FONKSİYONLARI (Round Robin, Pointer Ortalamalar)
 * ========================================================================= */

/*
 * Yazan: Muammer Eren Ozunlu
 * Açıklama: Tüm süreçlerin bitiş zamanları belli olduktan sonra, her süreç için
 * Dönüş Süresi (Bitiş - Geliş) ve Bekleme Süresi (Dönüş - CPU Süresi) değerlerini
 * hesaplayıp ilgili paralel dizilere kaydeder.
 */
void sureleri_hesapla(int n, int gelis[], int cpu[], int bitis[], int donus[], int bekleme[]) {
    int i;
    for (i = 0; i < n; i++) {
        donus[i] = bitis[i] - gelis[i];
        bekleme[i] = donus[i] - cpu[i];
    }
}

/*
 * Yazan: Muammer Eren Ozunlu
 * Açıklama: Bekleme ve dönüş sürelerinin ortalamasını hesaplar. C dilinde bir fonksiyon
 * return ile yalnızca tek bir değer döndürebildiği için, iki farklı sonucu (ortalama bekleme
 * ve ortalama dönüş süresi) adresle aktarım (pointer parametreler) kullanarak döndürür.
 */
void ortalamalar(int n, int bekleme[], int donus[], float *ort_bekleme, float *ort_donus) {
    int i;
    int toplam_bekleme = 0;
    int toplam_donus = 0;

    for (i = 0; i < n; i++) {
        toplam_bekleme += bekleme[i];
        toplam_donus += donus[i];
    }

    *ort_bekleme = (float)toplam_bekleme / n;
    *ort_donus = (float)toplam_donus / n;
}

/*
 * Yazan: Muammer Eren Ozunlu
 * Açıklama: Algoritma çalıştıktan sonra her sürecin numarasını, geliş zamanını,
 * CPU süresini, bitiş, dönüş ve bekleme sürelerini süreç numarası sırasıyla tablo halinde
 * ekrana yazdırır. Ardından ortalama bekleme, ortalama dönüş süresi (2 ondalık basamaklı)
 * ve toplam bağlam değişimi sayısını özet olarak basar.
 */
void sonuc_tablosu_yazdir(int n, int no[], int gelis[], int cpu[], int bitis[], int donus[], int bekleme[], float *algoritma_ort_bekleme) {
    int i;
    float ort_bek = 0.0f, ort_don = 0.0f;

    printf("\nSurec\tGelis\tCPU\tBitis\tDonus\tBekleme\n");
    printf("-----------------------------------------------\n");
    for (i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
               no[i], gelis[i], cpu[i], bitis[i], donus[i], bekleme[i]);
    }

    ortalamalar(n, bekleme, donus, &ort_bek, &ort_don);
    *algoritma_ort_bekleme = ort_bek;

    printf("\nOzet:\n");
    printf("Ortalama Bekleme Suresi : %.2f\n", ort_bek);
    printf("Ortalama Donus Suresi   : %.2f\n", ort_don);
    printf("Baglam Degisimi Sayisi  : %d\n", baglam_degisimi);
}

/*
 * Yazan: Muammer Eren Ozunlu
 * Açıklama: Tur tabanlı Round Robin zamanlama algoritmasını simüle eder.
 * Süreçler P1'den Pn'ye dairesel olarak dolaşılır. Sırası gelen süreç o anki saate kadar
 * gelmişse ve henüz bitmemişse en fazla zaman_dilimi (quantum) kadar çalışır.
 * Bir tur boyunca hiçbir süreç çalışmadıysa CPU boşta kalır; saat bitmemiş süreçlerin
 * en erken geliş zamanına ilerletilir, Gantt şemasına [BOS] bloğu eklenir ve yeni tur P1'den başlar.
 */
void round_robin_calistir(int n, int no[], int gelis[], int cpu[], int bitis[], int zaman_dilimi) {
    int i;
    int kalan_sure[MAKS_SUREC];
    int biten_sayisi = 0;

    for (i = 0; i < n; i++) {
        kalan_sure[i] = cpu[i];
    }

    while (biten_sayisi < n) {
        int bu_turda_calisan_oldu = 0;

        for (i = 0; i < n; i++) {
            if (gelis[i] <= saat && kalan_sure[i] > 0) {
                int calisma_suresi;
                int baslangic_saati = saat;

                if (kalan_sure[i] > zaman_dilimi) {
                    calisma_suresi = zaman_dilimi;
                } else {
                    calisma_suresi = kalan_sure[i];
                }

                saat += calisma_suresi;
                kalan_sure[i] -= calisma_suresi;
                bu_turda_calisan_oldu = 1;

                gantt_ekle(no[i], baslangic_saati, saat);

                if (kalan_sure[i] == 0) {
                    bitis[i] = saat;
                    biten_sayisi++;
                }
            }
        }

        /* Bir turda hiçbir süreç çalışmadıysa boşta kuralı uygulanır */
        if (bu_turda_calisan_oldu == 0 && biten_sayisi < n) {
            int en_erken_gelis = -1;
            for (i = 0; i < n; i++) {
                if (kalan_sure[i] > 0) {
                    if (en_erken_gelis == -1 || gelis[i] < en_erken_gelis) {
                        en_erken_gelis = gelis[i];
                    }
                }
            }

            if (en_erken_gelis > saat) {
                gantt_ekle(0, saat, en_erken_gelis);
                saat = en_erken_gelis;
            }
        }
    }
}

/* =========================================================================
 * BÖLÜM 4: ORTAK FONKSİYONLAR (Abdullah Arslan, Taha Yasin Karateke, Muammer Eren Özünlü)
 * ========================================================================= */

/*
 * Yazan: Abdullah Arslan, Taha Yasin Karateke, Muammer Eren Ozunlu (Ortak)
 * Açıklama: Üç algoritmanın ortalama bekleme sürelerini yan yana yazdırır ve
 * en düşük olanı belirler. Eşitlik durumunda FCFS, SJF, Round Robin sırasıyla ilk olanı seçer.
 */
void algoritmalari_karsilastir(float fcfs_bek, float sjf_bek, float rr_bek) {
    printf("\n===============================================\n");
    printf("         ALGORITMALARIN KARSILASTIRMASI        \n");
    printf("===============================================\n");
    printf("FCFS Ortalama Bekleme        : %.2f\n", fcfs_bek);
    printf("SJF Ortalama Bekleme         : %.2f\n", sjf_bek);
    printf("Round Robin Ortalama Bekleme : %.2f\n", rr_bek);

    if (fcfs_bek <= sjf_bek && fcfs_bek <= rr_bek) {
        printf("En Dusuk Ortalama Bekleme Suresi: FCFS (%.2f)\n", fcfs_bek);
    } else if (sjf_bek <= rr_bek) {
        printf("En Dusuk Ortalama Bekleme Suresi: SJF (%.2f)\n", sjf_bek);
    } else {
        printf("En Dusuk Ortalama Bekleme Suresi: Round Robin (%.2f)\n", rr_bek);
    }
}

/*
 * Yazan: Abdullah Arslan, Taha Yasin Karateke, Muammer Eren Ozunlu (Ortak)
 * Açıklama: Süreçleri bir kez okur, sırayla FCFS, SJF ve Round Robin algoritmalarını
 * çalıştırır ve sonuçlarını karşılaştırır.
 */
int main(void) {
    int n, zaman_dilimi;
    int no[MAKS_SUREC];
    int gelis[MAKS_SUREC];
    int cpu[MAKS_SUREC];
    int bitis[MAKS_SUREC];
    int donus[MAKS_SUREC];
    int bekleme[MAKS_SUREC];

    float fcfs_ort_bek = 0.0f;
    float sjf_ort_bek = 0.0f;
    float rr_ort_bek = 0.0f;

    /* 1. Süreç verilerini bir kez oku */
    surecleri_oku(&n, no, gelis, cpu, &zaman_dilimi);

    /* 2. FCFS Algoritması */
    printf("\n===============================================\n");
    printf("               1. FCFS ALGORITMASI             \n");
    printf("===============================================\n");
    sistemi_sifirla(n, bitis, donus, bekleme);
    fcfs_calistir(n, no, gelis, cpu, bitis);
    sureleri_hesapla(n, gelis, cpu, bitis, donus, bekleme);
    gantt_yazdir();
    sonuc_tablosu_yazdir(n, no, gelis, cpu, bitis, donus, bekleme, &fcfs_ort_bek);

    /* 3. SJF Algoritması */
    printf("\n===============================================\n");
    printf("               2. SJF ALGORITMASI              \n");
    printf("===============================================\n");
    sistemi_sifirla(n, bitis, donus, bekleme);
    sjf_calistir(n, no, gelis, cpu, bitis);
    sureleri_hesapla(n, gelis, cpu, bitis, donus, bekleme);
    gantt_yazdir();
    sonuc_tablosu_yazdir(n, no, gelis, cpu, bitis, donus, bekleme, &sjf_ort_bek);

    /* 4. Round Robin Algoritması */
    printf("\n===============================================\n");
    printf("      3. ROUND ROBIN ALGORITMASI (Q = %d)      \n", zaman_dilimi);
    printf("===============================================\n");
    sistemi_sifirla(n, bitis, donus, bekleme);
    round_robin_calistir(n, no, gelis, cpu, bitis, zaman_dilimi);
    sureleri_hesapla(n, gelis, cpu, bitis, donus, bekleme);
    gantt_yazdir();
    sonuc_tablosu_yazdir(n, no, gelis, cpu, bitis, donus, bekleme, &rr_ort_bek);

    /* 5. Karşılaştırma */
    algoritmalari_karsilastir(fcfs_ort_bek, sjf_ort_bek, rr_ort_bek);

    return 0;
}
