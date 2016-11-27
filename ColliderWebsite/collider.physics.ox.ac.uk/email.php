<?php
if(isset($_POST['selectage'])) {
     
    // EDIT THE 2 LINES BELOW AS REQUIRED
    $email_to = "marco.deltutto@physics.ox.ac.uk";
    $email_subject = "Collider Feedback Form";
     
     
    // validation expected data exists
    if(!isset($_POST['selectage']) ||
        !isset($_POST['selectedu']) ||
        !isset($_POST['radios1']) ||
        !isset($_POST['radios2']) ||
        !isset($_POST['radios3']) ||
        !isset($_POST['radios4'])) {
        echo "We are very sorry, but there were error(s) found with the form you submitted. ";  
        die;
    }
     
    $selectage = $_POST['selectage'];
    $selectedu = $_POST['selectedu'];
    $radios1 = $_POST['radios1'];
    $radios2 = $_POST['radios2'];
    $radios3 = $_POST['radios3'];
    $radios4 = $_POST['radios4'];
    $textinput = $_POST['textinput'];
     
    $email_message = "Form details below.\n\n";
     
    function clean_string($string) {
      $bad = array("content-type","bcc:","to:","cc:","href");
      return str_replace($bad,"",$string);
    }
     
    $email_message .= "Age: ".clean_string($selectage)."\n";
    $email_message .= "Education: ".clean_string($selectedu)."\n";
    $email_message .= "Interested in Particle Physics: ".clean_string($radios1)."\n";
    $email_message .= "Topics were interesting: ".clean_string($radios2)."\n";
    $email_message .= "Developed understanding: ".clean_string($radios3)."\n";
    $email_message .= "Encouraged to study further: ".clean_string($radios4)."\n";
    $email_message .= "Other Comments: ".clean_string($textinput)."\n";

     
// create email headers
$headers = "From: collider.developers@physics.ox.ac.uk\r\n".
"Reply-To: collider.developers@physics.ox.ac.uk\r\n" .
'X-Mailer: PHP/' . phpversion();
@mail($email_to, $email_subject, $email_message, $headers);  
?>

<!-- include your own success html here -->
 
Thank you for your feedback.
 
<?php
}
?>
