#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <errno.h>  
#include <time.h>  

char kullanici_adi[50];  

// Yemek bilgilerini tutan yapı
typedef struct {
    char yemek_adi[50];  
    int fiyat;  
    int hazirlama_suresi;  
    char durum[20];  
} YemekSepeti;

// Sipariş bilgilerini tutan yapı
typedef struct {
    char siparis_id[20];  
    char yemek_adi[50];  
    int fiyat; 
    char siparis_zamani[20]; 
    char hazirlanma_zamani[20];  
    char kullanici[50];  
    char asci[10];  
    char durum[20];  // Sipariş durumu
    char hazirlama_suresi[20];  // Yeni eklenen hazırlama süresi alanı
} Siparis;

// Mevcut yemeklerin listesini ekrana yazdırır
void mevcutYemekler() {
    FILE *file = fopen("C:\\Users\\OMEN\\Desktop\\C-yemeksepetiDeneme\\yemeklistesi.txt", "r"); 
    if (!file) {  
        fprintf(stderr, "Dosya acilamadi: %s\n", strerror(errno));
        return;
    }
    YemekSepeti yemek;  // Yemek bilgilerini tutmak için bir değişken
    int index = 1;  // Yemek listesindeki sırayı tutmak için değişken
    printf("Mevcut Yemekler:\n");  // Başlık yazdırır
    while (fscanf(file, "\n%49[^,],%d,%d,%19s", yemek.yemek_adi, &yemek.fiyat, &yemek.hazirlama_suresi, yemek.durum) == 4) {  // Yemek bilgilerini dosyadan okur
        if (strcmp(yemek.durum, "Mevcut") == 0) {  // Yemek durumu "Mevcut" ise ekrana yazdırır
            printf("%d. %s - %d TL - %d dakika\n", index++, yemek.yemek_adi, yemek.fiyat, yemek.hazirlama_suresi);
        }
    }
    fclose(file);  
}

// Sipariş ID'si üretir
void siparisID_uret(char* buffer) {    //Buffer = Ara Bellektir
    time_t now = time(NULL);  // Mevcut zamanı alır [now = şimdiki zaman]  
    struct tm *t = localtime(&now);  // Zamanı yerel zamana çevirir  [tm *t= Time Yani zaman değişkeni]
    strftime(buffer, 20, "SIP%y%m%d_%H%M%S", t);  // Zamanı belirli bir formatta string olarak yazdırır
}

// Geçerli zamanı alır
void getCurrentTime(char* buffer) {
    time_t now = time(NULL);  
    struct tm *t = localtime(&now);  
    strftime(buffer, 20, "%d.%m.%Y-%H.%M", t);  
}

// Zaman farkını dakika cinsinden hesapla
int kalanZaman(const char *orderTime, int prepTime) {  //orderTime  == Siparis ettiğimiz zaman , prepTime == Hazırlanma zamanı
    struct tm tm_order = {0};  // Sipariş zamanını tutmak için bir struct [tm_order = Siparis icin zaman(tarih-saat) türünden]
    int day, month, year, hour, minute;  // Sipariş zamanını ayrıştırmak için değişkenler

    // Zamanı elle ayrıştır
    sscanf(orderTime, "%d.%d.%d-%d.%d", &day, &month, &year, &hour, &minute);
    tm_order.tm_year = year - 1900;  // tm_year, 1900'den itibaren yılları sayar
    tm_order.tm_mon = month - 1;    // tm_mon, 0'dan itibaren ayları sayar
    tm_order.tm_mday = day;
    tm_order.tm_hour = hour;
    tm_order.tm_min = minute;
   

    time_t orderTime_t = mktime(&tm_order);  // Sipariş zamanını time_t türüne çevirir
    time_t toplamZaman = orderTime_t + prepTime * 60;  // Sipariş ettiğimiz zaman + Hazırlanma zamanı [60 ile carparak saniye türüne cevirir]
    time_t now = time(NULL);  // Şimdiki zamanı alır

    double saniyeler = difftime(toplamZaman, now);  // Şimdiki zaman (now) alınır ve difftime fonksiyonu ile hazırlanma süresi dahil edilmiş zaman ile şimdiki zaman arasındaki fark hesaplanır.
    return (int) saniyeler / 60;  // Kalan süreyi dakika cinsinden döner
}

// Yeni sipariş verir ve sipariş bilgilerini dosyaya yazar
void siparisVer(const char* kullanici_adi) {
    // Mevcut yemeklerin listesini tekrar yükle ve kullanıcıya seçim yaptır
    FILE *menuFile = fopen("C:\\Users\\OMEN\\Desktop\\C-yemeksepetiDeneme\\yemeklistesi.txt", "r");  
    if (!menuFile) {  
        fprintf(stderr, "Dosya acilamadi: %s\n", strerror(errno));
        return;
    }
    YemekSepeti yemeks[10];  // Menüde maksimum 10 yemek varsayıyoruz
    int yemekSayisi = 0;  // Mevcut yemek sayısını tutmak için değişken
    while (fscanf(menuFile, "\n%49[^,],%d,%d,%19s", yemeks[yemekSayisi].yemek_adi, &yemeks[yemekSayisi].fiyat, &yemeks[yemekSayisi].hazirlama_suresi, yemeks[yemekSayisi].durum) == 4) {  // Yemek bilgilerini dosyadan okur
        if (strcmp(yemeks[yemekSayisi].durum, "Mevcut") == 0) {  // Yemek durumu "Mevcut" ise sayacı artırır
            yemekSayisi++;
        }
    }
    fclose(menuFile);  

    printf("Siparis vermek istediginiz yemegin numarasini girin: "); 
    int yemekNum;
    scanf("%d", &yemekNum);
    yemekNum--;  // Listede gösterilen numaradan dizin değerine çevirme

    // Geçersiz seçim kontrolü
    if (yemekNum < 0 || yemekNum >= yemekSayisi) {  
        printf("Gecersiz secim. Lutfen gecerli bir numara girin.\n");
        return;
    }

    // Dosyaya sipariş bilgilerini yaz
    FILE *file = fopen("C:\\Users\\OMEN\\Desktop\\C-yemeksepetiDeneme\\siparisler.txt", "a");  
    if (!file) { 
        fprintf(stderr, "Dosya acilamadi: %s\n", strerror(errno));
        return;
    }

    char siparis_id[20]; 
    siparisID_uret(siparis_id);  // Sipariş ID'si üretir
    char siparis_zamani[20];  
    getCurrentTime(siparis_zamani);  // Geçerli zamanı alır

    // Hazırlanma zamanını hesapla
    struct tm tm_order = {0};  // Sipariş zamanını tutmak için bir struct
    int day, month, year, hour, minute;
    sscanf(siparis_zamani, "%d.%d.%d-%d.%d", &day, &month, &year, &hour, &minute);  // Zamanı ayrıştırır
    tm_order.tm_mon = month - 1;  // tm_mon, 0'dan başlayarak ayı ifade eder
    tm_order.tm_year = year - 1900;  // tm_year, 1900'den bu yana geçen yılları ifade eder
    tm_order.tm_mday = day;  // Günü ayarlar
    tm_order.tm_hour = hour;  // Saati ayarlar
    tm_order.tm_min = minute;  // Dakikayı ayarlar

    time_t orderTime_t = mktime(&tm_order);  // Sipariş zamanını time_t türüne çevirir
    time_t prepareTime_t = orderTime_t + yemeks[yemekNum].hazirlama_suresi * 60; 
    struct tm *t = localtime(&prepareTime_t);  
    char hazirlanma_zamani[20];  
    
    strftime(hazirlanma_zamani, sizeof(hazirlanma_zamani), "%d.%m.%Y-%H.%M", t);  // Hazırlık zamanını string olarak yazdırır
    fprintf(file, "%s,%s,%d,%s,%s,%s,%s,Hazirlaniyor,%d\n",
            siparis_id, yemeks[yemekNum].yemek_adi, yemeks[yemekNum].fiyat,
            siparis_zamani, hazirlanma_zamani,
            kullanici_adi, "AX", yemeks[yemekNum].hazirlama_suresi);  // Sipariş bilgilerini dosyaya yazar
    fclose(file); 
    printf("Siparis basariyla eklendi.\n");  
}

// Siparişleri ekrana yazdırır
void siparisyazdir(int k) {  // k, eğer 1 ise geçmiş siparişleri görüntüler, eğer 0 ise mevcut siparişleri görüntüler, k = kontrol
    int siparis_kontrol = 0;  // Sipariş bulunup bulunmadığını kontrol eder
    FILE *file = fopen("C:\\Users\\OMEN\\Desktop\\C-yemeksepetiDeneme\\siparisler.txt", "r+");
    if (!file) { 
        perror("Dosya acilamadı");
        return EXIT_FAILURE;
    }

    long position;
    Siparis siparis;  // Sipariş bilgilerini tutmak için bir struct
    while ((position = ftell(file)) >= 0 && fscanf(file, "%19[^,],%49[^,],%d,%19[^,],%19[^,],%49[^,],%9[^,],%19s\n",
                  siparis.siparis_id,
                  siparis.yemek_adi,
                  &siparis.fiyat,
                  siparis.siparis_zamani,
                  siparis.hazirlanma_zamani,
                  siparis.kullanici,
                  siparis.asci,
                  siparis.durum) == 8) {  // Sipariş bilgilerini dosyadan okur

        struct tm tm_hazirlanma = {0};  // Hazırlık zamanını tutmak için bir struct
        int day, month, year, hour, minute;
        sscanf(siparis.hazirlanma_zamani, "%d.%d.%d-%d.%d", &day, &month, &year, &hour, &minute);  // Hazırlık zamanını ayrıştırır
        tm_hazirlanma.tm_mday = day;
        tm_hazirlanma.tm_mon = month - 1;  // tm_mon, 0'dan başlar
        tm_hazirlanma.tm_year = year - 1900;  // tm_year, 1900'den itibaren sayılır
        tm_hazirlanma.tm_hour = hour;
        tm_hazirlanma.tm_min = minute;

        time_t hazirlanma_time = mktime(&tm_hazirlanma);  // Hazırlık zamanını time_t türüne çevirir
        time_t current_time = time(NULL);  // Mevcut zamanı alır

        if (difftime(current_time, hazirlanma_time) > 0) {  // Sipariş geçmiş sipariş ise
            if(strcmp(siparis.kullanici, kullanici_adi) == 0 && k == 1) {  // Kullanıcı adı uyuşuyor ve k = 1 ise
                siparis_kontrol = 1;
                printf("ID: %s, Yemek: %s, Fiyat: %d TL, Siparis Zamani: %s, Hazirlanma Zamani: %s, Kullanici: %s, Asci: %s, Durum: Tamamlandi\n",
                       siparis.siparis_id, siparis.yemek_adi, siparis.fiyat,
                       siparis.siparis_zamani, siparis.hazirlanma_zamani,
                       siparis.kullanici, siparis.asci);
            }
        } else {  // Sipariş aktif ise
            if(strcmp(siparis.kullanici, kullanici_adi) == 0 && (k == 0)) {  // Kullanıcı adı uyuşuyor ve k = 0 ise
                siparis_kontrol = 1;
                printf("ID: %s, Yemek: %s, Fiyat: %d TL, Siparis Zamani: %s, Hazirlanma Zamani: %s, Kullanici: %s, Asci: %s, Durum: Hazirlaniyor\n",
                       siparis.siparis_id, siparis.yemek_adi, siparis.fiyat,
                       siparis.siparis_zamani, siparis.hazirlanma_zamani,
                       siparis.kullanici, siparis.asci);
            }
        }
    }
    if(siparis_kontrol == 0) {  
        printf("Siparis bulunamadi.\n");
    }
}

int main() {
    int secim;  // Kullanıcının menüdeki seçimini tutmak için değişken
    printf("Kullanici adinizi girin: "); 
    scanf("%49s", kullanici_adi);

    do {
        printf("\nMenu:\n");
        printf("1. Yeni Siparis Ver\n");
        printf("2. Mevcut Siparis Durumunu Kontrol Et\n");
        printf("3. Onceki Siparislerimi Goruntule\n");
        printf("4. Cikis Yap\n");
        printf("Seciminizi girin: ");
        scanf("%d", &secim);

        switch (secim) {
            case 1:
                mevcutYemekler();  // Mevcut yemekleri listeler
                siparisVer(kullanici_adi);  // Yeni sipariş verir
                break;
            case 2:
                siparisyazdir(0);  // Mevcut sipariş durumunu kontrol eder
                break;
            case 3:
                siparisyazdir(1);  // Önceki siparişleri görüntüler
                break;
            case 4:
                printf("Cikis yapiliyor...\n");  
                break;
            default:
                printf("Gecersiz secim! Lutfen tekrar deneyin.\n");  
        }
    } while (secim != 4);  // Kullanıcı çıkış yapana kadar menüyü gösterir

    return 0;
}
