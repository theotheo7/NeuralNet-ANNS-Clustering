PROJECT 2024 - ΑΣΚΗΣΗ 3 - NeuralNet-ANNS-Clustering

ΟΜΑΔΑ 32
ΘΕΟΧΑΡΗΣ ΘΕΟΔΩΡΟΣ - 1115201800054
ΚΙΚΙΔΗΣ ΔΗΜΗΤΡΙΟΣ - 1115201600258

GITHUB REPO: https://github.com/theotheo7/NeuralNet-ANNS-Clustering

ΓΕΝΙΚΑ

Το project υλοποιήθηκε σε C++17 και με τη χρήση του github. Για τη
μεταγλώττιση υπάρχει διαθέσιμο Makefile.

Για τη μεταγλώττιση όλων των εκτελέσιμων:
    make all

Για τη μεταγλώττιση και εκτέλεση του lsh με default παραμέτρους:
    make lsh

Για τη μεταγλώττιση και εκτέλεση του hypercube με default παραμέτρους:
    make cube

Για τη μεταγλώττιση και εκτέλεση του GNNS με default παραμέτρους:
    make graph1

Για τη μεταγλώττιση και εκτέλεση του MRNG με default παραμέτρους:
    make graph2

Για τη μεταγλώττιση και εκτέλεση του clustering με default παραμέτρους:
    make cluster

Για τη μεταγλώττιση και εκτέλεση του clustering με reverseLSH:
    make clusterLSH

Για τη μεταγλώττιση και εκτέλεση του clustering με reverseCube:
    make clusterCube

ΝΕΟΣ ΔΙΑΝΥΣΜΑΤΙΚΟΣ ΧΩΡΟΣ:

Για τη μεταγλώττιση και εκτέλεση της αναζήτησης στον νέο χώρο με εξαντλητική αναζήτηση:
    make neural0

Για τη μεταγλώττιση και εκτέλεση της αναζήτησης στον νέο χώρο με GNNS:
    make neural1

Για τη μεταγλώττιση και εκτέλεση της αναζήτησης στον νέο χώρο με MRNG:
    make neural2

Για τη μεταγλώττιση και εκτέλεση του clustering στον νέο χώρο:
    make neural3

Για εκτέλεση ανεξάρτητα από το Makefile:
    ./build/<exec> <flags>

Για τον καθαρισμό όλων των εκτελέσιμων και των object files:
    make clean

O convolutional autoencoder υλοποιήθηκε στο google colab περιβάλλον με χρήση του tensorflow
και του keras. Αφού εκπαιδευτεί και τρέξει πάνω στα datasets κάνει export τα νέα αρχεία στο
google drive και από εκεί τα μεταφέρουμε στο project directory ώστε να κάνουμε τους πειραματισμούς
μας.

Χρησιμοποιήθηκαν όλα τα παραδοτέα από τα δύο προηγούμενα projects με τη μόνη διαφορά πως πλέον
για τις συγκρίσεις μας υπολογίζουμε το Mean Approximation Factor στις αναζητήσεις πλησιέστερου
γείτονα ενώ στο clustering, υπολογίζουμε και το objective function πέρα από το silhouette.

Για τα ζητούμενα στο νέο διανυσματικό χώρο δημιουργήσαμε μια νέα main, τη main_neuralnet.cpp,
καθώς και μία νέα κλάση τη NeuralNet (neuralnet.cpp/.hpp). Το νέο εκτελέσιμο που δημιουργούμε έχει την
εξής μορφή εκτέλεσης:

./build/neuralnet -d <path_to_input_initial> -i <path_to_input_latent> -q <path_to_query_initial>
-t <path_to_query_latent> -m (0-3) -c <path_to_cluster_conf> -o <path_to_output_file>

Το cluster conf χρειάζεται μόνο στην περίπτωση του clustering και το m παίρνει τις εξής τιμές:

0: Exhaustive search
1: GNNS
2: MRNG
3: Clustering (lloyds)

Η μόνη διαφορά που έχει η μεθοδολογία μας στα πειραματά μας στο νέο διανυσματικό χώρο, είναι πως
αφού κάνουμε τους υπολογισμούς μας και βρούμε τον πλησιέστερο γείτονα ή ολοκληρώσουμε το clustering,
προτού υπολογίσουμε τις τιμές για τη σύγκριση, κάνουμε αναγωγή στον αρχικό χώρο. Για αυτό δεν χρησι-
μοποιούμε το neural network, αλλά αντιθέτως παίρνουμε το index ενός image από το νέο input file και
απλά βρίσκουμε το αρχικό image από το αρχικό input file. Τα images είναι πάντα με την ίδια σειρά οπότε
δεν υπάρχει περίπτωση λάθους. Όλα τα αποτελέσματα των πειραμάτων στο νέο διανυσματικό χώρο, εκτυπώνονται
by default στο αρχείο resources/outputNeural.txt.

Η νέα κλάση που δημιουργήθηκε εξυπηρετεί την εξαντλητική αναζήτηση, ενώ για GNNS και MRNG προσαρμόστηκαν τα
προηγούμενα παραδοτέα.