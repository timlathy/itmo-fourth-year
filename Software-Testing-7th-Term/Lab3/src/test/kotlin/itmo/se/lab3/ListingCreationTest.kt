package itmo.se.lab3

import itmo.se.lab3.helpers.*
import itmo.se.lab3.pages.*
import org.junit.jupiter.api.Assertions.*
import org.junit.jupiter.api.TestTemplate
import org.junit.jupiter.api.extension.ExtendWith
import org.openqa.selenium.WebDriver

@ExtendWith(WebDriverTemplateInvocationContextProvider::class)
class ListingCreationTest {
    @TestTemplate
    @RunWithChrome
    @RunWithFirefox
    fun `it allows users to submit new articles to marketplace`(driver: WebDriver) {
        val homePage = HomePage(driver)
        homePage.logIn(UserCredentials.email, UserCredentials.password)

        val emptyOwnListingsPage = ListingManagementPage(driver)
        emptyOwnListingsPage.listItems().forEach { it.removeOwnListing() }

        val newListingPage = ListingCreationPage(driver)
        newListingPage.titleField.sendKeys("(Черновик) Шляподевочка")
        newListingPage.categorySelector.selectByVisibleText("IT, софт")
        newListingPage.textTypeSelector.selectByVisibleText("Перевод")

        newListingPage.textField.sendKeys("[desc]Шляподевочка довольна энергична и проста как ребенок[/desc]. Это в основном проявляется в невинности ее высказываний и простодушных анимаций. Она также показала себя довольно вредной, особенно с мафией, например, когда она сбивает Громил Мафии с уступов в городе мафии, или когда она украла зонт у мафии в «Добро пожаловать в город мафии», или когда она сжигала картины, содержащие души захваченных людей в огне, чтобы помочь Огненным духам.")
        newListingPage.descriptionField.sendKeys("Состояние — work in progress. Статья будет отредактирована перед выставлением на продажу или удалена из системы.")
        newListingPage.priceField.sendKeys("25")

        newListingPage.acceptConditionsCheckbox.click()
        newListingPage.submitButton.click()

        val ownListingsPage = ListingManagementPage(driver)
        val listings = ownListingsPage.listItems()
        assertEquals(1, listings.size)
        assertEquals("(Черновик) Шляподевочка", listings[0].title)
        assertEquals("Состояние — work in progress. Статья будет отредактирована перед выставлением на продажу или удалена из системы.", listings[0].description)
        assertEquals(27.5, listings[0].price) // including the fee
    }
}