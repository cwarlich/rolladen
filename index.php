<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
       "http://www.w3.org/TR/html4/strict.dtd">
<html>
    <head>
        <title>Rolladensteuerung</title>
        <style>
            .images { 
                position: relative; 
                width: 100%; /* for IE 6 */
            }
            .text { 
                position: absolute; 
                top: 70%; 
                left: 0px;
                width: 100%;
                font-size: 400%;
            }
            .center {
                margin-left:auto;
                margin-right:auto;
                width:262px;
            }
        </style>
    </head>
    <body>
        <form method="get" action="">
            <?php
                $w=237;
                foreach (explode(",", "Links,Rechts,Terrasse,terrasse,Küche,Wohnen,pergola,Pergola,Arbeit,Gäste,Maja,Lukas,Schlafen,West,Nord,Bad,Flur,Kinder") as $x) {
                    echo "<p>";
                    echo "<button style=\"float:left\" name=\"var\" type=\"submit\" value=\"$x rauf\">";
                        echo "<p><img src=\"arrow_up.png\" width=\"$w\" height=\"$w\" alt=\"rauf\"></p>";
                    echo "</button>";
                    echo "<button style=\"float:right\" name=\"var\" type=\"submit\" value=\"$x runter\">";
                        echo "<p><img src=\"arrow_down.png\" width=\"$w\" height=\"$w\" alt=\"runter\"></p>";
                    echo "</button>";
                    echo "<div class=\"center\">";
                    echo "<button name=\"var\" type=\"submit\" value=\"$x halt\">";
                        echo "<div class=\"images\">";
                            echo "<p><img src=\"stop.png\" width=\"$w\" height=\"$w\" alt=\"$x\"></p>";
                            echo "<span class=\"text\">$x</span>";
                        echo "</div>";
                    echo "</button>";
                    echo "</div>";
                    echo "</p>";
	        }
	    ?>
        </form>
        <?php
	    exec("./rollo " . $_GET["var"]);
	?>
    </body>
</html>
