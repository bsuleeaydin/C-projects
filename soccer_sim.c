#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

// SABITLER
#define GENISLIK 80
#define YUKSEKLIK 25
#define OYUNCU_SAYISI 16 // 8 Kirmizi, 8 Siyah

#define TOP_MESAFESI 1.3    // Top kapma mesafesi
#define PRES_MESAFESI 8     // Pres yapma mesafesi
#define HIZ 0.4             // Oyuncu hareket hizi
#define TOP_HIZ 0.5         // Top hizi

// --- VERI YAPILARI (STRUCT) ---

typedef enum {
    TOP_DURUM_SERBEST,
    TOP_DURUM_OYUNCUDA,
    TOP_DURUM_GOL_SONRASI,
    TOP_DURUM_ORTA_VURUS_BEKLIYOR
} TopDurumu;

typedef struct {
    float x, y;         // Hassas konum
    float dx, dy;       // Hiz vektörü
    int sahipID;  
    TopDurumu durum;    // Top kimde? -1: Ortada, 0-15: Oyuncu ID'si
    int golBeklemeSuresi;     
} Top;

typedef struct {
    int id;
    float x, y;
    float baslangicX, baslangicY; // Formasyon pozisyonu
    int takim;          // 0: Kırmızı, 1: Siyah
    char sembol;        // Ekranda görünen harf (1-8)
} Oyuncu;

//---GLOBAL DEGISKENLER---
int kirmiziSkor = 0;
int siyahSkor = 0;
int devre = 1;
int sure = 0;
int macBasladi = 0;
int golSonrasiBekleme = 0; // gol sonrasi bekleme
char spikerMesaj[100] = "MAC BASLAMAYA HAZIR...";
char zeminTampon[YUKSEKLIK][GENISLIK]; // Sahanın kopyasını tutar
Top top; // Global Top değişkeni

// --- FONKSIYON PROTOTIPLERI ---
void ayarlar();
void gotoxy(int x, int y);
void renkDegistir(int renkKodu);
void sahaCiz();
void oyunDongusu();
void oyunculariOlustur(Oyuncu oyuncular[]);
int topHareketi(Top *top, Oyuncu oyuncular[]);
void oyuncuHareketi(Oyuncu *oyuncu, Top *top, Oyuncu oyuncular[]);
void spikerGuncelle(Top *top, int golTakim);
void yesilTemizle(int x, int y);
void kaleciHareketi(Oyuncu *kaleci, Top *top);


// --- MAIN ---
int main() {
    ayarlar();      // Konsol ayarları
    sahaCiz();      // Sahayı 1 kere çiz
    oyunDongusu();  // Sonsuz döngü başlasın
    return 0;
}

// --- OYUN MANTIGI ---
void oyunDongusu() {
    // Global degiskeni baslatıyoruz 
    top.x = 40; //sahanin ortasi
    top.y = 12;
    top.dx = 0; 
    top.dy = 0; 
    top.sahipID = -1;
    top.durum = TOP_DURUM_SERBEST;

    Oyuncu oyuncular[OYUNCU_SAYISI];
    oyunculariOlustur(oyuncular);

    srand(time(NULL));
    
    int oyunDevam = 1;
    int golOldu = 0;
    int golBekleme = 0;
    
    // Baslangicta topu merkezde goster
    gotoxy((int)top.x, (int)top.y);
    renkDegistir(15);
    printf("O");
    
    // Baslangic ekrani
    renkDegistir(32);
    gotoxy(15, 3);
    printf("BASLAMAK ICIN SPACE");

    // SPACE tusuna basilmasini bekle (BASLANGIC)
    while (!macBasladi) {
        // Topu her frame merkezde goster (hareketsiz)
        gotoxy((int)top.x, (int)top.y);
        renkDegistir(15);
        printf("O");
        
        if (kbhit()) {
            char tus = getch();
            if (tus == ' ') {
                macBasladi = 1;
                // Baslangic yazisini sil
                gotoxy(15, 3);
                printf("\033[42m                    \033[0m"); //rengin yesil kalmasi 
                // Topa baslangic hizi ver
                top.dx = (rand() % 3 - 1) * 0.3;
                top.dy = (rand() % 3 - 1) * 0.2;
                strcpy(spikerMesaj, "ORTA VURUS! MAC BASLADI!");
            } else if (tus == 27) {
                return; // ESC ile cik
            }
        }
        Sleep(50);
    }

    while (oyunDevam) {
        /* === ESKI KONUMU SIL === */
        // Once oyunculari sil
        renkDegistir(32);
        for (int i = 0; i < OYUNCU_SAYISI; i++) {
            yesilTemizle((int)oyuncular[i].x, (int)oyuncular[i].y);
        }
        // Sonra topu sil
        yesilTemizle((int)top.x, (int)top.y);


        /* === GOL SONRASI BEKLEME MODU === */
        if (golOldu) {
            golBekleme++;
            
            // 2 saniye sonra "Space bekleniyor" mesajıini goster
            if (golBekleme > 20) { // 20*50ms = 1 saniye
                // Topu merkezde goster
                gotoxy(40, 12);
                renkDegistir(15);
                printf("O");
                
                // Space mesajini goster
                renkDegistir(32);
                gotoxy(15, 3);
                printf("DEVAM ETMEK ICIN SPACE");
            }
            
            // Space tusuna basilirsa devam et
            if (kbhit()) {
                char tus = getch();
                if (tus == ' ' && golBekleme > 40) {
                    golOldu = 0;
                    golBekleme = 0;
                    
                    oyunculariOlustur(oyuncular);//oyuncular ilk diziliminde olsun
                    // Topu ortaya koy
                    top.x = 40;
                    top.y = 12;
                    top.dx = 0;
                    top.dy = 0;
                    top.sahipID = -1;
                    top.durum = TOP_DURUM_SERBEST; // Top serbest moda geçsin
                    
                    // Yaziyi temizle
                    gotoxy(15, 3);
                    printf("                        ");
                    
                    strcpy(spikerMesaj, "ORTA VURUS! OYUN TEKRAR BASLIYOR!");
                    
                    // 1 saniye bekle ve topa vur
                    Sleep(1000);
                    top.dx = (rand() % 3 - 1) * 0.3;
                    top.dy = (rand() % 3 - 1) * 0.2;
                    if(top.dx == 0 && top.dy == 0) top.dx = 0.3;
                    // Bu frame'i atla ki top hemen görünsün
                    continue;
                }
            }
            
            Sleep(50);
            continue; // Gol sonrasi beklemede diğer islemleri yapma
        }

        /* === NORMAL OYUN MANTIĞI === */
        topHareketi(&top, oyuncular);

        /* === OYUNCU HAREKETI === */
        for (int i = 0; i < OYUNCU_SAYISI; i++) {
            oyuncuHareketi(&oyuncular[i], &top, oyuncular);
        }

        /* === TOP KAPMA KONTROLU === */
        if (top.sahipID == -1) {
            for (int i = 0; i < OYUNCU_SAYISI; i++) {
                float dx = top.x - oyuncular[i].x;
                float dy = top.y - oyuncular[i].y;
                float mesafe = sqrt(dx*dx + dy*dy);
                
                if (mesafe < TOP_MESAFESI) {
                    // %50 sansla topu al
                    if (rand() % 100 < 50) {
                        top.sahipID = i;
                        // Topu oyuncunun onune al
                        if (oyuncular[i].takim == 0) {
                            top.x = oyuncular[i].x + 1;
                        } else {
                            top.x = oyuncular[i].x - 1;
                        }
                        top.y = oyuncular[i].y;
                        strcpy(spikerMesaj, "TOP KAPILDI!");
                        break;
                    }
                }
            }
        }

        /* === GOL KONTROLU === */
        int golTakim = -1;
        if (top.x <= 1 && top.y >= 8 && top.y <= 18) { // Sag kale gol
            golTakim = 1; // Siyah takim gol atti
            siyahSkor++;
            golOldu = 1;
            // Topu durdur ve merkeze getir
            top.x = 40;
            top.y = 12;
            top.dx = 0;
            top.dy = 0;
            strcpy(spikerMesaj, "GOOOL! SIYAH TAKIM SKOR YAPTI!");
        } else if (top.x >= 78 && top.y >= 8 && top.y <= 18) { // Sol kale gol
            golTakim = 0; // Kirmizi takim gol atti
            kirmiziSkor++;
            golOldu = 1;
            // Topu durdur ve merkeze getir
            top.x = 40;
            top.y = 12;
            top.dx = 0;
            top.dy = 0;
            strcpy(spikerMesaj, "GOOOL! KIRMIZI TAKIM SKOR YAPTI!");
        }
        
        if (golTakim != -1) {
            spikerGuncelle(&top, golTakim);
        }

        /* === OYUNCULARI CIZ (ÖNCE) === */
        for (int i = 0; i < OYUNCU_SAYISI; i++) {
            gotoxy((int)oyuncular[i].x, (int)oyuncular[i].y);
            if (oyuncular[i].takim == 0) {
                renkDegistir(12); // Kirmizi oyuncu
            } else {
                renkDegistir(8);  // Gri (siyah takim)
            }
            printf("%c", oyuncular[i].sembol);
        }

        /* === TOPU CIZ (SONRA) === */
        gotoxy((int)top.x, (int)top.y);
        renkDegistir(15); // Beyaz top
        printf("O");

        /* === SKOR VE BILGI === */
        renkDegistir(32); // Yesil zemin, siyah yazi
        
        // Skor ve zaman bilgisi
        gotoxy(1, 0);
        printf("SKOR: [ KRM %d - %d SYH ]  %d. YARI  %02d:00  (ESC: Cikis)", 
               kirmiziSkor, siyahSkor, devre, sure / 20);
        // Spiker mesaji
        gotoxy(1, YUKSEKLIK);
        printf("SPOR SPIKERI: %s", spikerMesaj);

        /* === ZAMAN VE DEVRE MANTIGI === */
        Sleep(50); // Her döngü 50ms (Saniyede 20 kare)
        sure++;

        // 20 döngü = 1 saniye (Gerçek) = 1 Dakika (Oyun)
        int oyunDakikasi = sure / 20;

        // --- İLK YARI BİTİŞİ (45. Dakika) ---
        if (devre == 1 && oyunDakikasi >= 45) {
            devre = 2;
            sure = 0; // Süreyi sifirla (2. yari 0'dan baslasin)
            
            // Oyunu biraz duraksatip mesaj verelim
            strcpy(spikerMesaj, "ILK YARI BITTI! IKINCI DEVRE BASLIYOR...");
            
            // Topu ve takimlari sifirlamak (Orta vurus icin)
            top.x = 40; top.y = 12; 
            top.dx = 0; top.dy = 0;
            top.durum = TOP_DURUM_ORTA_VURUS_BEKLIYOR; // Santra beklemesi
            
            // 2 saniye bekletip devam et
            Sleep(2000); 
            strcpy(spikerMesaj, "IKINCI DEVRE BASLADI!");
        }

        // --- MAC BITISI (2. Yarinin 45. Dakikasi) ---
        if (devre == 2 && oyunDakikasi >= 45) {
            oyunDevam = 0;
            strcpy(spikerMesaj, "MAC SONA ERDI!");
        }

        // ESC ile cikis
        if (kbhit()) {
            char tus = getch();
            if (tus == 27) oyunDevam = 0;
        }
    }
    
    // Mac sonu
    gotoxy(30, YUKSEKLIK / 2);
    if (kirmiziSkor > siyahSkor) {
        printf("KIRMIZI TAKIM KAZANDI! %d-%d", kirmiziSkor, siyahSkor);
    } else if (siyahSkor > kirmiziSkor) {
        printf("SIYAH TAKIM KAZANDI! %d-%d", kirmiziSkor, siyahSkor);
    } else {
        printf("MAC BERABERE BITTI! %d-%d", kirmiziSkor, siyahSkor);
    }
    
    getch();
}

void yesilTemizle(int x, int y) {
    DWORD yazilan;
    COORD pos = {x, y};
    
    // Hafizadan o noktada olmasi gereken karakteri al
    char eskiKarakter = ' '; // Varsayilan bosluk
    
    if(x >= 0 && x < GENISLIK && y >= 0 && y < YUKSEKLIK) {
        eskiKarakter = zeminTampon[y][x];
    }
    
    // Eğer hafızadaki karakter null veya bozuksa bosluk yap
    if (eskiKarakter == 0) eskiKarakter = ' ';

    // Karakteri ekrana geri yaz (Çizgiyi onar)
    FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), eskiKarakter, 1, pos, &yazilan);
    
    // Rengi yesil zemin yap (32 = Yeşil Zemin)
    FillConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 32, 1, pos, &yazilan);
}

//Top 

int topHareketi(Top *top, Oyuncu oyuncular[]) {
    int golYapanTakim = -1;  // -1: gol yok, 0: kirmizi, 1: siyah
    
    // DURUMA GÖRE DAVRANIS
    switch(top->durum) {
        
        case TOP_DURUM_GOL_SONRASI:
            top->golBeklemeSuresi--;
            
            // Bekleme bitti, orta vurus beklemeye gec
            if (top->golBeklemeSuresi <= 0) {
                top->durum = TOP_DURUM_ORTA_VURUS_BEKLIYOR;
                top->x = 40;
                top->y = 12;
                top->dx = 0;
                top->dy = 0;
                strcpy(spikerMesaj, "ORTA VURUS ICIN SPACE TUSUNA BASIN");
            }
            break;
            
        case TOP_DURUM_ORTA_VURUS_BEKLIYOR:
            // Space tusunu kontrol et 
            // Ya da burada kbhit kontrolu yapilabilir
            break;
            
        case TOP_DURUM_SERBEST:
            // Fiziksel hareket
            top->x += top->dx;
            top->y += top->dy;
            
            // carpma ve sürtünme 
            if (top->y <= 2 || top->y >= 24) {
                top->dy *= -0.8;
                strcpy(spikerMesaj, "TAC!");
            }
            if (top->x <= 2 || top->x >= 78) {
                top->dx *= -0.8;
                // GOL KONTROLU
                if (top->x <= 1 && top->y >= 8 && top->y <= 18) {
                    golYapanTakim = 1;  // Siyah takim gol
                    top->durum = TOP_DURUM_GOL_SONRASI;
                    top->golBeklemeSuresi = 80;  // 4 saniye
                    strcpy(spikerMesaj, "GOOOL! SIYAH TAKIM!");
                    return golYapanTakim;
                } 
                else if (top->x >= 78 && top->y >= 8 && top->y <= 18) {
                    golYapanTakim = 0;  // Kirmizi takim gol
                    top->durum = TOP_DURUM_GOL_SONRASI;
                    top->golBeklemeSuresi = 80;
                    strcpy(spikerMesaj, "GOOOL! KIRMIZI TAKIM!");
                    return golYapanTakim;
                }
                else if (top->y >= 8 && top->y <= 18) {
                    strcpy(spikerMesaj, "KORNER!");
                } else {
                    strcpy(spikerMesaj, "TAC!");
                }
            }
            
            // Sürtünme
            top->dx *= 0.995;
            top->dy *= 0.995;
            
            // Minimum hiz
            if (fabs(top->dx) < 0.01) top->dx = 0;
            if (fabs(top->dy) < 0.01) top->dy = 0;
            
            // TOP KAPMA KONTROLU
            for (int i = 0; i < OYUNCU_SAYISI; i++) {
                float dx = top->x - oyuncular[i].x;
                float dy = top->y - oyuncular[i].y;
                float mesafe = sqrt(dx*dx + dy*dy);
                
                if (mesafe < TOP_MESAFESI && rand() % 100 < 50) {
                    top->sahipID = i;
                    top->durum = TOP_DURUM_OYUNCUDA;
                    strcpy(spikerMesaj, "TOP KAPILDI!");
                    break;
                }
            }
            break;
            
        case TOP_DURUM_OYUNCUDA:
            // Topu oyuncuya bagla
            if (oyuncular[top->sahipID].takim == 0) {
                top->x = oyuncular[top->sahipID].x + 1;
            } else {
                top->x = oyuncular[top->sahipID].x - 1;
            }
            top->y = oyuncular[top->sahipID].y;
            
            // Oyuncu topu birakabilir (sut vb.)
            break;
    }
    
    return golYapanTakim;  // -1 donerse gol yok
}

void oyuncuHareketi(Oyuncu *oyuncu, Top *top, Oyuncu oyuncular[]) {
    // Kaleci kontrolu 
    if (oyuncu->sembol == '1') { 
        kaleciHareketi(oyuncu, top);
        return;
    }

    float hedefX, hedefY;
    int benimGercekID = (oyuncu->takim == 0 ? oyuncu->id : oyuncu->id + 8);

    // DURUM 1: TOP BENDE (HÜCUM MODU)
    if (top->sahipID == benimGercekID) {
        
        // --- A) SUT / PAS MEKANİZMASI ---
        // Her karede %5 şansla topu elinden çikarir (Pas veya sut)
        if (rand() % 100 < 5) { 
            
            top->sahipID = -1; // Topu serbest birak
            top->durum = TOP_DURUM_SERBEST;
            
            // YÖN BELİRLEME: Kendi kaleme degil, rakip kaleye vur!
            // Kirmizi (0) -> Saga (+), Siyah (1) -> Sola (-)
            float vurusYonu = (oyuncu->takim == 0 ? 1.0f : -1.0f);
            
            // HIZ VER (Sert vursun)
            top->dx = vurusYonu * 0.9f; 
            top->dy = (rand() % 20 - 10) * 0.05f; // Rastgele kavis

            // !!! KRİTİK: TOPU UZAĞA FIRLAT (Vakum Engelleme) !!!
            top->x = oyuncu->x + (vurusYonu * 3.0f); 
            
            strcpy(spikerMesaj, (rand()%2 == 0 ? "HARIKA BIR PAS!" : "SERT SUT!"));
            return; // Vurus yaptı, bu tur hareket etmesin
        }

        // --- B) TOP SURME (Dribbling) ---
        // Rakip kaleye git
        float hedefKaleX = (oyuncu->takim == 0) ? 78.0f : 2.0f; 
        float dx = hedefKaleX - oyuncu->x;
        float dy = 13.0f - oyuncu->y; // Kale merkezine git
        float mesafe = sqrt(dx*dx + dy*dy);

        // Topu süren adam YAVAS kossun (0.6 kati) ki defans yetissin
        if (mesafe > 0.5) {
            oyuncu->x += (dx / mesafe) * (HIZ * 0.6); 
            oyuncu->y += (dy / mesafe) * (HIZ * 0.6);
        }

        // Topu oyuncunun dibine yapıstır 
        top->x = (oyuncu->takim == 0) ? oyuncu->x + 1.2 : oyuncu->x - 1.2;
        top->y = oyuncu->y;
        
        return; // Top bendeyse başka hesap yapma, cık.
    }

    
    // DURUM 2: TOP BENDE DEGIL (SAVUNMA / POZISYON ALMA)
    
    // --- BOLGE TANIMLAMA (Her oyuncunun görev yeri) ---
    float bolgeMinX, bolgeMaxX;
    
    // Kirmizi Takim (Soldan Saga oynar)
    if (oyuncu->takim == 0) { 
        if (oyuncu->id >= 1 && oyuncu->id <= 3) { // DEFANS
            bolgeMinX = 2; bolgeMaxX = 40; 
        } else if (oyuncu->id >= 4 && oyuncu->id <= 6) { // ORTA SAHA
            bolgeMinX = 20; bolgeMaxX = 60;
        } else { // FORVET
            bolgeMinX = 40; bolgeMaxX = 78;
        }
    } 
    // Siyah Takim (Sagdan Sola oynar)
    else { 
        if (oyuncu->id >= 1 && oyuncu->id <= 3) { // DEFANS
            bolgeMinX = 40; bolgeMaxX = 78;
        } else if (oyuncu->id >= 4 && oyuncu->id <= 6) { // ORTA SAHA
            bolgeMinX = 20; bolgeMaxX = 60;
        } else { // FORVET
            bolgeMinX = 2; bolgeMaxX = 40;
        }
    }

    // Top benim bolgemde mi?
    bool topBolgemde = (top->x >= bolgeMinX && top->x <= bolgeMaxX);
    
    // Cok uzaktaki topa koşmasin (Y ekseninde koridor farkı)
    if (fabs(top->y - oyuncu->baslangicY) > 15.0f) {
        topBolgemde = false;
    }

    // --- KARAR VERME ---
    if (topBolgemde && top->sahipID == -1) {
        // 1. Top sahipsiz ve yakinimda -> KOS AL!
        hedefX = top->x;
        hedefY = top->y;
    } 
    else if (topBolgemde && top->sahipID != -1 && oyuncular[top->sahipID].takim != oyuncu->takim) {
        // 2. Top rakipte ve bolgemde -> PRES YAP!
        hedefX = top->x;
        hedefY = top->y;
    } 
    else {
        // 3. Top uzakta -> KENDI YERINE DON
        hedefX = oyuncu->baslangicX;
        hedefY = oyuncu->baslangicY;

        // Taktik: Top yukarıdaysa ben de biraz yukari kayayım (Bütün takım topa göre kayar)
        float kaymaY = (top->y - 12) * 0.5; 
        hedefY += kaymaY;
    }

    // --- HAREKETI UYGULA ---
    float dx = hedefX - oyuncu->x;
    float dy = hedefY - oyuncu->y;
    float mesafe = sqrt(dx*dx + dy*dy);

    if (mesafe > 0.5) {
        // Hedefe dogru normal hizda kos
        oyuncu->x += (dx / mesafe) * HIZ;
        oyuncu->y += (dy / mesafe) * HIZ;
    } else {
        // === CANLILIK EFEKTI (IDLE ANIMATION) ===
        // Eğer hedefine vardiysa (mesafe < 0.5), put gibi durmasin.
        // Oldugu yerde hafifce kipirdasin 
        oyuncu->x += (rand() % 3 - 1) * 0.1; // Hafif sag-sol titresim
        oyuncu->y += (rand() % 3 - 1) * 0.1; // Hafif yukari-asagı
    }

    // Saha sinirlari kontrolu
    if (oyuncu->x < 2) oyuncu->x = 2;
    if (oyuncu->x > 77) oyuncu->x = 77;
    if (oyuncu->y < 2) oyuncu->y = 2;
    if (oyuncu->y > 23) oyuncu->y = 23;
}

// ===KALECİ HAREKETI ===
void kaleciHareketi(Oyuncu *kaleci, Top *top) {
    // === SINIRLAR (Ceza Sahasi Kutusu) ===
    float minX, maxX, minY, maxY;

    // Y Sinirlari (Kale direkleri arasi + biraz pay)
    minY = 8.0f; 
    maxY = 18.0f;

    if (kaleci->takim == 0) { // Kirmizi (Sol Kale)
        minX = 2.0f;  
        maxX = 10.0f; // Ceza sahasi çizgisi
    } else { // Siyah (Sag Kale)
        minX = 70.0f; // Ceza sahasi cizgisi
        maxX = 77.0f; 
    }

    // === HEDEF BELIRLEME ===
    float hedefX = kaleci->x; // X'te genelde sabit kal
    float hedefY = top->y;    // Y'de topu takip et

    // Eger top cok uzaktaysa (tehlike yoksa) kaleyi ortala
    bool tehlikeVar = (kaleci->takim == 0 && top->x < 40) || (kaleci->takim == 1 && top->x > 40);
    
    if (!tehlikeVar) {
        hedefY = 13.0f; // Merkeze don
        // X olarak da cizgisinde beklesin
        hedefX = (kaleci->takim == 0) ? 5.0f : 75.0f;
    } else {
        // Tehlike varsa topa dogru biraz çikilabilir (fakat max sinirda durur)
        hedefX = top->x;
    }

    // === SINIRLARA HAPSETME (CLAMP) ===
    // Burasi kalecinin o kutudan asla çikmamasini saglar
    if (hedefX < minX) hedefX = minX;
    if (hedefX > maxX) hedefX = maxX;
    if (hedefY < minY) hedefY = minY;
    if (hedefY > maxY) hedefY = maxY;

    // === HAREKET ===
    float dx = hedefX - kaleci->x;
    float dy = hedefY - kaleci->y;
    float mesafe = sqrt(dx*dx + dy*dy);

    if (mesafe > 0.5) {
        // Kaleci digerlerinden biraz daha yavas/kontrollu hareket etsin
        kaleci->x += (dx / mesafe) * (HIZ * 0.6); 
        kaleci->y += (dy / mesafe) * (HIZ * 0.6);
    }
}

void spikerGuncelle(Top *top, int golTakim) {
    const char* golIfadeler[] = {
        "VE GOOOL! KIRMIZI TAKIM SKORU ACTI!",
        "GOOOOL! SIYAH TAKIM SKOR YAPTI!",
        "INANILMAZ BIR GOL!",
        "KALE DUSTU! GOOL!",
        "MUHTESEM BIR VURUS! GOOL!"
    };
    
    int rastgele = rand() % 5;
    
    if (golTakim == 0) {
        sprintf(spikerMesaj, "%s (Kirmizi: %d - Siyah: %d)", 
                golIfadeler[rastgele], kirmiziSkor, siyahSkor);
    } else {
        sprintf(spikerMesaj, "%s (Kirmizi: %d - Siyah: %d)", 
                golIfadeler[rastgele], kirmiziSkor, siyahSkor);
    }
}

void oyunculariOlustur(Oyuncu o[]) {
    // Kirmizi Takim (Sol Taraf) -> ID 0-7
    // Siyah Takim (Sag Taraf) -> ID 8-15
    
    // Kirmizi Takim (1-8 numaralari)
    o[0] = (Oyuncu){0, 5, 12, 5, 12, 0, '1'}; // Kaleci
    o[1] = (Oyuncu){1, 15, 6, 15, 6, 0, '2'};
    o[2] = (Oyuncu){2, 15, 18, 15, 18, 0, '3'};
    o[3] = (Oyuncu){3, 25, 4, 25, 4, 0, '4'};
    o[4] = (Oyuncu){4, 25, 12, 25, 12, 0, '5'};
    o[5] = (Oyuncu){5, 25, 20, 25, 20, 0, '6'};
    o[6] = (Oyuncu){6, 35, 10, 35, 10, 0, '7'};
    o[7] = (Oyuncu){7, 35, 14, 35, 14, 0, '8'};

    // Siyah Takim (Simetrik) (1-8 numaralari)
    o[8] = (Oyuncu){1, 75, 12, 75, 12, 1, '1'}; // Kaleci
    o[9] = (Oyuncu){2, 65, 6, 65, 6, 1, '2'};
    o[10] = (Oyuncu){3, 65, 18, 65, 18, 1, '3'};
    o[11] = (Oyuncu){4, 55, 4, 55, 4, 1, '4'};
    o[12] = (Oyuncu){5, 55, 12, 55, 12, 1, '5'};
    o[13] = (Oyuncu){6, 55, 20, 55, 20, 1, '6'};
    o[14] = (Oyuncu){7, 45, 10, 45, 10, 1, '7'};
    o[15] = (Oyuncu){8, 45, 14, 45, 14, 1, '8'};
}

// --- YARDIMCI FONKSIYONLAR ---

void ayarlar() {
    // İmleci gizle
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void renkDegistir(int renkKodu) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), renkKodu);
}

// Hem ekrana yazar hem hafızaya kaydeder
void zeminYaz(int x, int y, int asciiKod) {
    gotoxy(x, y);
    printf("%c", asciiKod);
    // (Koordinat sınırlarını kontrol et)
    if(x >= 0 && x < GENISLIK && y >= 0 && y < YUKSEKLIK) {
        zeminTampon[y][x] = (char)asciiKod;
    }
}

void sahaCiz() {
    system("cls");
    system("color 20"); // Yesil zemin, siyah yazi

    // Once tum hafizayi boslukla doldur
    for(int y=0; y<YUKSEKLIK; y++) {
        for(int x=0; x<GENISLIK; x++) {
            zeminTampon[y][x] = ' ';
        }
    }

    int x, y;

    /* DIS ÇERÇEVE  */
    zeminYaz(1,1, 218);
    for(x=2; x<GENISLIK-1; x++) zeminYaz(x, 1, 196); // Ust cizgi
    zeminYaz(GENISLIK-1, 1, 191);

    for(y=2; y<YUKSEKLIK-1; y++){
        zeminYaz(1, y, 179);
        zeminYaz(GENISLIK-1, y, 179);
    }

    zeminYaz(1, YUKSEKLIK-1, 192);
    for(x=2; x<GENISLIK-1; x++) zeminYaz(x, YUKSEKLIK-1, 196);
    zeminYaz(GENISLIK-1, YUKSEKLIK-1, 217);

    /* ===== ORTA SAHA CIZGISI ===== */
    for(y=2; y<YUKSEKLIK-1; y++){
        zeminYaz(GENISLIK/2, y, 179);
    }

    /* ===== ORTA SAHA DIKDORTGENI ===== */
    int ortaSol   = GENISLIK/2 - 6;
    int ortaSag   = GENISLIK/2 + 6;
    int ortaUst   = 9;
    int ortaAlt   = 17;

    for(x = ortaSol; x <= ortaSag; x++){
        zeminYaz(x, ortaUst, 196);
        zeminYaz(x, ortaAlt, 196);
    }
    for(y = ortaUst; y <= ortaAlt; y++){
        zeminYaz(ortaSol, y, 179);
        zeminYaz(ortaSag, y, 179);
    }
    zeminYaz(ortaSol, ortaUst, 218);
    zeminYaz(ortaSag, ortaUst, 191);
    zeminYaz(ortaSol, ortaAlt, 192);
    zeminYaz(ortaSag, ortaAlt, 217);

    /* ===== SOL KALE DIKDORTGENI ===== */
    for(y=8; y<=18; y++){
        zeminYaz(1, y, 179);
        zeminYaz(10, y, 179);
    }
    for(x=1; x<=10; x++){
        zeminYaz(x, 8, 196);
        zeminYaz(x, 18, 196);
    }
    zeminYaz(1, 8, 218);
    zeminYaz(10, 8, 191);
    zeminYaz(1, 18, 192);
    zeminYaz(10, 18, 217);

    /* ===== SAG KALE DIKDORTGENI ===== */
    for(y=8; y<=18; y++){
        zeminYaz(GENISLIK-1, y, 179);
        zeminYaz(GENISLIK-10, y, 179);
    }
    for(x=GENISLIK-10; x<=GENISLIK-1; x++){
        zeminYaz(x, 8, 196);
        zeminYaz(x, 18, 196);
    }
    zeminYaz(GENISLIK-10, 8, 218);
    zeminYaz(GENISLIK-1, 8, 191);
    zeminYaz(GENISLIK-10, 18, 192);
    zeminYaz(GENISLIK-1, 18, 217);

    /* ===== SKOR VE ZAMAN GOSTERIMI ===== */
    gotoxy(1, 0);
    printf("SKOR: [ KRM 0 - 0 SYH ]  1. YARI  00:00  (ESC: Cikis)");
}
